#include "nvmem.h"

#include "Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_def.h"
#include "Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal_flash.h"

#define CEILING(x, y) (((x) + (y)-1) / (y))

#define NV_FLASH_START_ADDR 0x08030000
#define NV_FLASH_BLOCK_SIZE 16u
#define NV_FLASH_BLOCK_NUM 1000u  // (0x08033e80 end addr using this size, check boundaries before resizing)

#define FLASH_MEM_START 0x08000000
#define FLASH_FIRST_PAGE (NV_FLASH_START_ADDR - FLASH_MEM_START) / 2048u
#define FLASH_PAGE_COUNT CEILING(NV_FLASH_BLOCK_SIZE* NV_FLASH_BLOCK_NUM, 2048u)

#define NV_END_ADDR NV_FLASH_START_ADDR + (NV_FLASH_BLOCK_SIZE * NV_FLASH_BLOCK_NUM)

typedef struct
{
    uint16_t id;
    uint16_t block;  // 16 bit allow to cover FFFF0 range of addresses, change to expand
} __attribute__((packed)) cacheEntry;

typedef union
{
    struct
    {
        uint16_t marker;   // equal to 0xeeee
        uint16_t invalid;  // 0x0001 = invalid, 0x0000 = valid
        uint32_t id;       // 0 ~ 0xffff
        uint64_t data;
    } data;

    struct
    {
        uint64_t MSB;
        uint64_t LSB;
    } raw;

} __attribute__((packed)) memBlock;

cacheEntry cache[NV_FLASH_BLOCK_NUM];
uint32_t cacheSize = 0;
uint32_t flashSize = 0;  // in blocks

uint32_t nvFormat()
{
    FLASH_EraseInitTypeDef eraseinfo;
    eraseinfo.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseinfo.Banks     = FLASH_BANK_1;
    eraseinfo.Page      = FLASH_FIRST_PAGE;  // start from NV_FLASH_START_ADDR
    eraseinfo.NbPages   = FLASH_PAGE_COUNT;

    uint32_t err = 0;

    HAL_FLASH_Unlock();
    HAL_StatusTypeDef ret = HAL_FLASHEx_Erase(&eraseinfo, &err);
    HAL_FLASH_Lock();

    cacheSize = 0;
    flashSize = 0;

    return ret == HAL_OK ? NV_OK : NV_ERR_ERASE;
}

void _updateCache(uint16_t id, uint16_t block)
{
    for (uint32_t i = 0; i < cacheSize; i++)
    {
        if (cache[i].id == id)
        {
            // update
            cache[i].block = block;
            return;
        }
    }

    // create
    cache[cacheSize] = (cacheEntry){.id = id, .block = block};
    cacheSize++;
}

void _cacheErase(uint32_t index)
{
    for (uint32_t i = index; i < (cacheSize - 1); i++) cache[i] = cache[i + 1];
    cacheSize--;
}

void _removeFromCache(uint16_t id)
{
    for (uint32_t i = 0; i < cacheSize; i++)
    {
        if (cache[i].id == id) _cacheErase(i);
    }
}

// return false if not found, true otherwise
bool _getBlock(uint16_t id, uint16_t* block)
{
    for (uint32_t i = 0; i < cacheSize; i++)
    {
        if (cache[i].id == id)
        {
            // update
            *block = cache[i].block;
            return true;
        }
    }

    return false;
}

void _get(memBlock* mem, uint32_t addr)
{
    uint64_t* p = (uint64_t*)addr;

    mem->raw.LSB = *p;
    p++;
    mem->raw.MSB = *p;
}

bool _set(memBlock* mem, uint32_t addr)
{
    HAL_FLASH_Unlock();
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, mem->raw.LSB) != HAL_OK) return false;
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr + 8, mem->raw.MSB) != HAL_OK) return false;
    HAL_FLASH_Lock();
    return true;
}

bool _isValidId(uint32_t id) { return id <= 0xffff; }

uint32_t _blockToAddr(uint16_t block) { return NV_FLASH_START_ADDR + (block * NV_FLASH_BLOCK_SIZE); }

uint32_t nvCleanup()
{
    if (cacheSize == NV_FLASH_BLOCK_NUM) return NV_ERR_MEM_FULL;

    uint64_t data[NV_FLASH_BLOCK_NUM];
    memBlock temp;

    for (uint32_t i = 0; i < cacheSize; i++)
    {
        _get(&temp, _blockToAddr(cache[i].block));
        data[i]        = temp.data.data;
        cache[i].block = i;  // reorder sequentially
    }

    // wipe the flash
    uint32_t ret = nvFormat();
    if (ret != NV_OK) return ret;

    temp.data.marker  = 0xeeee;
    temp.data.invalid = 0;

    for (uint32_t i = 0; i < cacheSize; i++)
    {
        temp.data.id   = cache[i].id;
        temp.data.data = data[i];

        if (!_set(&temp, _blockToAddr(cache[i].block))) return NV_ERR_WRITE;
    }

    flashSize = cacheSize;

    return NV_OK;
}

uint32_t nvInit()
{
    memBlock temp;
    flashSize = 0;
    cacheSize = 0;

    for (uint16_t block = 0; block < NV_FLASH_BLOCK_NUM; block++)
    {
        _get(&temp, _blockToAddr(block));

        if (!_isValidId(temp.data.id)) break;  // end of used memory reached (not incrementing flashSize)
        if (!temp.data.invalid && temp.data.marker == 0xeeee) _updateCache(temp.data.id, block);

        flashSize++;
    }

    return cacheSize;
}

uint32_t nvRead(uint32_t id, uint64_t* data)
{
    if (!_isValidId(id)) return NV_ERR_INVALID_ID;

    uint16_t block = 0;
    if (!_getBlock(id, &block)) return NV_ERR_NOTFOUND;

    memBlock temp;
    _get(&temp, _blockToAddr(block));

    *data = temp.data.data;

    return NV_OK;
}

uint32_t nvWrite(uint32_t id, uint64_t data)
{
    if (!_isValidId(id)) return NV_ERR_INVALID_ID;

    memBlock temp;

    // avoid rewrite data if is equal to the stored one
    uint16_t block = 0;
    if (_getBlock(id, &block))
    {
        _get(&temp, _blockToAddr(block));
        if (temp.data.data == data) return NV_OK;
    }

    // check if there is room for the new entry
    if (flashSize == NV_FLASH_BLOCK_NUM)
    {
        uint32_t ret = nvCleanup();
        if (ret != NV_OK) return ret;
    }

    // write
    temp.data.marker  = 0xeeee;
    temp.data.invalid = 0;
    temp.data.id      = id;
    temp.data.data    = data;

    _updateCache(id, flashSize);
    if (!_set(&temp, _blockToAddr(flashSize))) return NV_ERR_WRITE;

    flashSize++;

    return NV_OK;
}

uint32_t nvRemove(uint32_t id)
{
    if (!_isValidId(id)) return NV_ERR_INVALID_ID;

    // avoid invalidating if already done
    uint16_t block = 0;
    if (!_getBlock(id, &block)) return NV_OK;  // not present in cache

    // check if there is room for the new invalidation entry
    if (flashSize == NV_FLASH_BLOCK_NUM)
    {
        uint32_t ret = nvCleanup();
        if (ret != NV_OK) return ret;
    }

    memBlock temp;

    // write
    temp.data.marker  = 0xeeee;
    temp.data.invalid = 1;  // invalidate
    temp.data.id      = id;
    temp.data.data    = 0x00;  // do not care

    if (!_set(&temp, _blockToAddr(flashSize))) return NV_ERR_WRITE;

    _removeFromCache(id);
    flashSize++;

    return NV_OK;
}