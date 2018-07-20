/*
   Copyright (c) 2017 TOSHIBA Digital Solutions Corporation

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "util/container.h"

namespace util {


InsertionResetter::~InsertionResetter() {
	reset();
}

void InsertionResetter::release() throw() {
	entry_ = Entry();
}

void InsertionResetter::reset() throw() {
	if (entry_.func_ != NULL) {
		entry_.func_(entry_.container_, entry_.pos_);
		release();
	}
}

InsertionResetter::Entry::Entry() :
		func_(NULL), container_(NULL), pos_(0) {
}

InsertionResetter::Entry::Entry(
		ResetterFunc func, void *container, size_t pos) :
		func_(func), container_(container), pos_(pos) {
}

} 
