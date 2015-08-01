/**
 * sparseengine.cpp - The entry of sparseengine
 *
 * Copyright (c) 2014-2015 angersax@gmail.com
 *
 * This file is part of Fs Explorer.
 *
 * Fs Explorer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fs Explorer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Fs Explorer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sparseengine.h"

#define SPARSE_HEADER_MAGIC 0xed26ff3a
#define COPY_BUF_SIZE (1024*1024)
#define SPARSE_HEADER_MAJOR_VER 1

#define CHUNK_TYPE_RAW       0xCAC1
#define CHUNK_TYPE_FILL      0xCAC2
#define CHUNK_TYPE_DONT_CARE 0xCAC3
#define CHUNK_TYPE_CRC32     0xCAC4

typedef struct sparse_header {
  uint32_t magic;          /* 0xed26ff3a */
  uint16_t major_version;  /* (0x1) - reject images with higher major versions */
  uint16_t minor_version;  /* (0x0) - allow images with higer minor versions */
  uint16_t file_hdr_sz;    /* 28 bytes for first revision of the file format */
  uint16_t chunk_hdr_sz;   /* 12 bytes for first revision of the file format */
  uint32_t blk_sz;         /* block size in bytes, must be a multiple of 4 (4096) */
  uint32_t total_blks;     /* total blocks in the non-sparse output image */
  uint32_t total_chunks;   /* total chunks in the sparse input image */
  uint32_t image_checksum; /* CRC32 checksum of the original data, counting "don't care" */
                           /* as 0. Standard 802.3 polynomial, use a Public Domain */
                           /* table implementation */
} sparse_header_t;

typedef struct chunk_header {
  uint16_t chunk_type; /* 0xCAC1 -> raw; 0xCAC2 -> fill; 0xCAC3 -> don't care */
  uint16_t reserved1;
  uint32_t chunk_sz;   /* in blocks in output image */
  uint32_t total_sz;   /* in bytes of chunk input file including chunk header and data */
} chunk_header_t;

const uint32_t SparseEngine::crc32Tab[] = {
  0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
  0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
  0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
  0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
  0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
  0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
  0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
  0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
  0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
  0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
  0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
  0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
  0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
  0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
  0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
  0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
  0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
  0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
  0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
  0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
  0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
  0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
  0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
  0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
  0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
  0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
  0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
  0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
  0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
  0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
  0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
  0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
  0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
  0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
  0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
  0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
  0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
  0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
  0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
  0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
  0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
  0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
  0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

SparseEngine::SparseEngine(const QString &name)
{
  srcFile = new QFile(name);
  (void)srcFile->open(QIODevice::ReadOnly);

  dstFile = new QTemporaryFile();
  dstFile->setAutoRemove(false);
  (void)dstFile->open();

  copyBuf = new uint8_t[COPY_BUF_SIZE];
}

SparseEngine::~SparseEngine()
{
  if (copyBuf) {
    delete[] copyBuf;
    copyBuf = NULL;
  }

  if (dstFile) {
    dstFile->close();
    delete dstFile;
    dstFile = NULL;
  }

  if (srcFile) {
    srcFile->close();
    delete srcFile;
    srcFile = NULL;
  }
}

bool SparseEngine::isSparseFile(const QString &name)
{
  QFile file(name);
  sparse_header_t header;
  qint64 ret;
  bool status;

  if (!file.open(QIODevice::ReadOnly)) {
    return false;
  }

  if (!file.seek(0)) {
    status = false;
    goto isSparseFileExit;
  }

  memset(&header, 0, sizeof(header));
  ret = file.read((char *)&header, sizeof(header));
  if (ret < 0) {
    status = false;
    goto isSparseFileExit;
  }

  if (header.magic != SPARSE_HEADER_MAGIC) {
    status = false;
    goto isSparseFileExit;
  }

  status = true;

 isSparseFileExit:

  file.close();

  return status;
}

int SparseEngine::count()
{
  return 0;
}

const QString SparseEngine::unsparseFile()
{
  return dstFile->fileName();
}

void SparseEngine::process()
{
  QString dstName = dstFile->fileName();
  sparse_header_t sparse_header;
  chunk_header_t chunk_header;
  uint32_t crc32 = 0;
  uint32_t total_blocks = 0;
  unsigned int i;
  int ret;
  bool status;

  if (!dstFile->seek(0)) {
    status = false;
    goto processExit;
  }

  ret = readAll(&sparse_header, sizeof(sparse_header));
  if (ret != sizeof(sparse_header)) {
    status = false;
    goto processExit;
  }

  if (sparse_header.magic != SPARSE_HEADER_MAGIC) {
    status = false;
    goto processExit;
  }

  if (sparse_header.major_version != SPARSE_HEADER_MAJOR_VER) {
    status = false;
    goto processExit;
  }

  if (sparse_header.file_hdr_sz > sizeof(sparse_header_t)) {
    /*
     * Skip the remaining bytes in a header that is longer than
     * we expected.
     */
    (void)srcFile->seek(sparse_header.file_hdr_sz - sizeof(sparse_header_t));
  }

  for (i = 0; i < sparse_header.total_chunks; i++) {
    ret = readAll(&chunk_header, sizeof(chunk_header));
    if (ret != sizeof(chunk_header)) {
      status = false;
      goto processExit;
    }

    if (sparse_header.chunk_hdr_sz > sizeof(chunk_header_t)) {
      /*
       * Skip the remaining bytes in a header that is longer than
       * we expected.
       */
      (void)srcFile->seek(sparse_header.chunk_hdr_sz - sizeof(chunk_header_t));
    }

    switch (chunk_header.chunk_type) {
    case CHUNK_TYPE_RAW:
      if (chunk_header.total_sz != (sparse_header.chunk_hdr_sz +
                                    (chunk_header.chunk_sz * sparse_header.blk_sz))) {
        status = false;
        goto processExit;
      }
      total_blocks += processRawChunk(chunk_header.chunk_sz,
                                      sparse_header.blk_sz,
                                      &crc32);
      break;
    case CHUNK_TYPE_FILL:
      if (chunk_header.total_sz != (sparse_header.chunk_hdr_sz + sizeof(uint32_t))) {
        status = false;
        goto processExit;
      }
      total_blocks += processFillChunk(chunk_header.chunk_sz,
                                       sparse_header.blk_sz,
                                       &crc32);
      break;
    case CHUNK_TYPE_DONT_CARE:
      if (chunk_header.total_sz != sparse_header.chunk_hdr_sz) {
        status = false;
        goto processExit;
      }
      total_blocks += processSkipChunk(chunk_header.chunk_sz,
                                       sparse_header.blk_sz,
                                       &crc32);
      break;
    case CHUNK_TYPE_CRC32:
      ret = processCrc32Chunk(crc32);
      if (ret) {
        status = false;
        goto processExit;
      }
      break;
    default:
      status = false;
      goto processExit;
    }
  }

  /*
   * If the last chunk was a skip, then the code just did a seek, but
   * no write, and the file won't actually be the correct size.  This
   * will make the file the correct size.  Make sure the offset is
   * computed in 64 bits, and the function called can handle 64 bits.
   */
  if (!dstFile->resize((uint64_t)total_blocks * sparse_header.blk_sz)) {
    status = false;
    goto processExit;
  }

  if (sparse_header.total_blks != total_blocks) {
    status = false;
    goto processExit;
  }

  if (!dstFile->flush()) {
    status = false;
    goto processExit;
  }

  status = true;

 processExit:

  dstFile->close();
  srcFile->close();

  if (status) {
    emit message(QString(tr("Completed.")));
  } else {
    (void)QFile::remove(dstName);
    dstName.clear();
    emit message(QString(tr("Aborted.")));
  }

  emit finished();
}

int SparseEngine::readAll(void *buf, size_t len)
{
  char *ptr = (char *)buf;
  size_t total = 0;
  int ret;

  while (total < len) {
    ret = srcFile->read(ptr, len - total);

    if (ret < 0) {
      return ret;
    }

    if (ret == 0) {
      return total;
    }

    ptr += ret;
    total += ret;
  }

  return total;
}

int SparseEngine::writeAll(void *buf, size_t len)
{
  char *ptr = (char *)buf;
  size_t total = 0;
  int ret;

  while (total < len) {
    ret = dstFile->write(ptr, len - total);

    if (ret < 0) {
      return ret;
    }

    if (ret == 0) {
      return total;
    }

    ptr += ret;
    total += ret;
  }

  return total;
}

uint32_t SparseEngine::sparseCrc32(uint32_t crc_in, const void *buf, int size)
{
  const uint8_t *p = (const uint8_t *)buf;
  uint32_t crc;

  crc = crc_in ^ ~0U;

  while (size--) {
    crc = crc32Tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
  }

  return crc ^ ~0U;
}

int SparseEngine::processRawChunk(uint32_t blocks, uint32_t blk_sz, uint32_t *crc32)
{
  uint64_t len = (uint64_t)blocks * blk_sz;
  int chunk;
  int ret;

  while (len) {
    chunk = (int)((len > COPY_BUF_SIZE) ? COPY_BUF_SIZE : len);

    ret = readAll(copyBuf, chunk);
    if (ret != chunk) {
      return -1;
    }

    *crc32 = sparseCrc32(*crc32, copyBuf, chunk);

    ret = writeAll(copyBuf, chunk);
    if (ret != chunk) {
      return -1;
    }

    len -= chunk;
  }

  return blocks;
}

int SparseEngine::processFillChunk(uint32_t blocks, uint32_t blk_sz, uint32_t *crc32)
{
  unsigned int i;
  uint64_t len = (uint64_t)blocks * blk_sz;
  int chunk;
  uint32_t fill_val;
  uint32_t *fillbuf = NULL;
  int ret;

  (void)readAll(&fill_val, sizeof(fill_val));
  fillbuf = (uint32_t *)copyBuf;

  for (i = 0; i < (COPY_BUF_SIZE / sizeof(fill_val)); i++) {
    fillbuf[i] = fill_val;
  }

  while (len) {
    chunk = (int)((len > COPY_BUF_SIZE) ? COPY_BUF_SIZE : len);

    *crc32 = sparseCrc32(*crc32, copyBuf, chunk);

    ret = writeAll(copyBuf, chunk);
    if (ret != chunk) {
      return -1;
    }

    len -= chunk;
  }

  return blocks;
}

int SparseEngine::processSkipChunk(uint32_t blocks, uint32_t blk_sz, uint32_t *crc32)
{
  /*
   * len needs to be 64 bits, as the sparse file specifies the skip amount
   * as a 32 bit value of blocks.
   */
  uint64_t len = (uint64_t)blocks * blk_sz;
  crc32 = crc32;

  (void)dstFile->seek(len);

  return blocks;
}

int SparseEngine::processCrc32Chunk(uint32_t crc32)
{
  uint32_t file_crc32;
  int ret;

  ret = readAll(&file_crc32, 4);
  if (ret != 4) {
    return -1;
  }

  if (file_crc32 != crc32) {
    return -1;
  }

  return 0;
}
