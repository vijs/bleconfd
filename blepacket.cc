//
// Copyright [2018] [jacobgladish@yahoo.com]
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include "blepacket.h"

#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

static uint16_t kFirstFrameBit          = {0x0001};
static uint16_t kContinuationFrameBit   = {0x0002};
static uint16_t kFrameVersion           = {0x7};

BLEPacket::Fragmenter::Fragmenter(char const* s, int n, int fragment_size)
    : m_data(reinterpret_cast<uint8_t const *>(s))
    , m_index(0)
    , m_header(0)
    , m_length(n)
    , m_fragment_size(fragment_size)
{
}

bool
BLEPacket::Fragmenter::nextFrameWithHeader(uint8_t* buff, int* n)
{
  uint8_t const* p = nullptr;
  uint32_t header = 0;

  memset(buff, 0, *n);

  bool b = nextFrame(&p, n, &header);

  memcpy(buff, &header, sizeof(header));
  memcpy(buff + sizeof(header), p, *n);

  return b;
}

bool
BLEPacket::Fragmenter::nextFrame(uint8_t const** p, int* n, uint32_t* header)
{
  if (!p || !n)
    return false;

  assert(m_index <= m_length);
  if (m_index == m_length)
    return false;

  if (header)
  {
    *header = 0;
    *header |= (kFrameVersion << 28);
    if (m_index == 0)
      *header |= (kFirstFrameBit << 16);
    else
      *header |= (kContinuationFrameBit << 16);
  }

  // account for header
  int payload_length = m_fragment_size;
  if ((m_index + m_fragment_size) > m_length)
    payload_length = (m_length - m_index);

  *p = m_data + m_index;
  *n = payload_length;

  m_index += payload_length;

  if (header)
    *header |= htons(*n & 0x0000ffff);

  return true;
}

void
BLEPacket::printHeader(int n)
{
  uint16_t flags = (n >> 16 & 0xffff);
  printf("[");
  for (int i = 0; i < 16; ++i)
  {
    printf("%0d", (flags >> (15 - i)) & 1);
  }
  printf("]");
  printf("%05d\n", frameLengthFromHeader(n));
}

uint16_t
BLEPacket::frameLengthFromHeader(uint16_t n)
{
  return ntohs(0x0000ffff & n);
}

bool
BLEPacket::isContinuationFrame(uint32_t header)
{
  return ((0x00ff0000 & header) >> 15) > 1;
}


bool
BLEPacket::isFirstFrame(uint32_t header)
{
  return (0x00ff0000 & header) >> 16 == 1;
}


bool
BLEPacket::isContainsHeader(uint8_t* frame)
{
  int header = *reinterpret_cast<int*>(frame);
  return header >> 28 == kFrameVersion && isFirstFrame(header);
}

void
BLEPacket::printFrame(uint8_t* frame)
{
  int* header = reinterpret_cast<int*>(frame);
  printf("---> HDR:");
  printHeader(*header);
  printf("---> PKT:%.*s\n", frameLengthFromHeader(*header), (char*) (frame + 4));
}