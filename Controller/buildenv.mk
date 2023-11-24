SGX = 0
SGX_ARCH ?= x64
SGXSDK := /opt/intel/sgxsdk

ifneq ($(SGX_MODE), HW)
        URTS_LIB_NAME := sgx_urts_sim
else
        URTS_LIB_NAME := sgx_urts
endif

ifeq ($(shell getconf LONG_BIT), 32)
        SGX_ARCH := x86
else ifeq ($(findstring -m32, $(CXXFLAGS)), -m32)
        SGX_ARCH := x86
endif

ifeq ($(SGX_ARCH), x86)
        SGX_COMMON_FLAGS := -m32
        SGX_LIBRARY_PATH := $(SGXSDK)/lib
        SGX_ENCLAVE_SIGNER := $(SGXSDK)/bin/x86/sgx_sign
        SGX_EDGER8R := $(SGXSDK)/bin/x86/sgx_edger8r
else
        SGX_COMMON_FLAGS := -m64
        SGX_LIBRARY_PATH := $(SGXSDK)/lib64
        SGX_ENCLAVE_SIGNER := $(SGXSDK)/bin/x64/sgx_sign
        SGX_EDGER8R := $(SGXSDK)/bin/x64/sgx_edger8r
endif

