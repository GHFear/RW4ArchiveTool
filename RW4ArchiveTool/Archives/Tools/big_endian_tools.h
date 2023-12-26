#pragma once

template <typename T, uint8_t Size>
struct UINTTypeSelector;

template <typename T>
struct UINTTypeSelector<T, 2> {
  using Type = uint16_t;
};

template <typename T>
struct UINTTypeSelector<T, 4> {
  using Type = uint32_t;
};

template <typename T>
struct UINTTypeSelector<T, 8> {
  using Type = uint64_t;
};

template <typename T>
typename UINTTypeSelector<T, sizeof(T)>::Type BigToLittleUINT(T input) {
  using ReturnType = typename UINTTypeSelector<T, sizeof(T)>::Type;

  if constexpr (sizeof(T) == 2) {
    return static_cast<ReturnType>((input & 0xFF) << 8) | ((input >> 8) & 0xFF);
  }
  else if constexpr (sizeof(T) == 4) {
    return static_cast<ReturnType>((input & 0xFF) << 24) | (((input >> 8) & 0xFF) << 16) |
      (((input >> 16) & 0xFF) << 8) | ((input >> 24) & 0xFF);;
  }
  else if constexpr (sizeof(T) == 8) {
    return static_cast<ReturnType>((input & 0xFFULL) << 56) | (((input >> 8) & 0xFFULL) << 48) |
      (((input >> 16) & 0xFFULL) << 40) | (((input >> 24) & 0xFFULL) << 32) |
      (((input >> 32) & 0xFFULL) << 24) | (((input >> 40) & 0xFFULL) << 16) |
      (((input >> 48) & 0xFFULL) << 8) | ((input >> 56) & 0xFFULL);;
  }
  else {
    // Unsupported size
    static_assert(sizeof(T) == 0, "Unsupported integer size");
  }
}

// Make a hexadecimal string from a hexadecimal array.
void bytearray_to_hexstring(const unsigned char* byteArray, size_t byteArraySize, char* hexString) {
  for (size_t i = 0; i < byteArraySize; ++i) {
    snprintf(&hexString[i * 2], 3, "%02X", byteArray[i]);
  }
}

uint64_t roundUpToMultiple(uint64_t value, uint64_t multiple) {
  if (multiple == 0) {
    // Avoid division by zero
    return value;
  }

  // Calculate the remainder
  uint64_t remainder = value % multiple;

  // If the remainder is non-zero, round up
  if (remainder != 0) {
    value += (multiple - remainder);
  }

  return value;
}

bool CreateDirectoryRecursively(const wchar_t* path) {
  // Attempt to create the directory
  if (CreateDirectory(path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
    return true;
  }

  // If creation failed and the error is because the directory already exists, return true
  if (GetLastError() == ERROR_ALREADY_EXISTS) {
    return true;
  }

  // If the error is because of missing parent directories, create them recursively
  if (GetLastError() == ERROR_PATH_NOT_FOUND) {
    size_t pos = std::wstring(path).find_last_of(L"\\/");
    if (pos != std::wstring::npos) {
      std::wstring parentPath = std::wstring(path).substr(0, pos);
      if (!CreateDirectoryRecursively(parentPath.c_str())) {
        return false;
      }

      // Retry creating the directory after creating the parent directories
      return CreateDirectory(path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
    }
  }

    return false;
}

uint64_t hash(const char* str) // Hash used for big archives.
{
  uint64_t hash = 5381;
  int c;

  while (c = *str++)
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}