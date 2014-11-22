#
#    Copyright (C) 2014 Haruki Hasegawa
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#

#
# definitions
#

# NOTE: this function is used to avoid MATH_PATH(=260) issue on Windows...
lfds-normalize-path = $(shell $(HOST_PYTHON) -c "import os.path; print os.path.abspath(\"$(firstword $1)\").replace(\"\\\\\", \"/\")")

#
# lockfreedatastructure
#
MY_DIR := $(call my-dir)
LFDS_TOP_DIR := $(call lfds-normalize-path, $(MY_DIR)/../../../..)

LOCAL_PATH := $(LFDS_TOP_DIR)

include $(CLEAR_VARS)

LOCAL_MODULE     := lockfreedatastructure_static
LOCAL_MODULE_FILENAME := lockfreedatastructure

LOCAL_SRC_FILES  := 	# no source files

LOCAL_CFLAGS := -DLFDS_HAVE_CXXPORTHELPER=1
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_EXPORT_CFLAGS := -DLFDS_HAVE_CXXPORTHELPER=1
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

include $(BUILD_STATIC_LIBRARY)
