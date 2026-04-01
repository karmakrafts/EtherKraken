// Copyright 2026 Karma Krafts
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "kraken_array_list.h"
#include "kraken_alloc.h"
#include "kraken_error_impl.h"

kraken_error_t kraken_array_list_create(const size_t initial_capacity, const size_t element_size,
                                        kraken_array_list_t* list) {
    KRAKEN_CHECK_PTR(list, KRAKEN_ERR_INVALID_ARG, "Invalid array list pointer");
    KRAKEN_CHECK(element_size > 0, KRAKEN_ERR_INVALID_ARG, "Invalid array list element size");
    list->memory = kraken_calloc(element_size * initial_capacity);// May allocate size 0
    KRAKEN_CHECK_PTR(list->memory, KRAKEN_ERR_INVALID_OP, "Could not allocate array list memory");
    list->size = 0;
    list->capacity = initial_capacity;
    list->element_size = element_size;
    return KRAKEN_OK;
}

kraken_error_t kraken_array_list_alloc(const size_t initial_capacity, const size_t element_size,
                                       kraken_array_list_t** list) {
    KRAKEN_CHECK_PTR(list, KRAKEN_ERR_INVALID_ARG, "Invalid array list address pointer");
    kraken_array_list_t* new_list = kraken_calloc(sizeof(kraken_array_list_t));
    KRAKEN_CHECK_ERROR(kraken_array_list_create(initial_capacity, element_size, new_list),
                       "Could not initialize array list");
    *list = new_list;
    return KRAKEN_OK;
}

kraken_error_t kraken_array_list_add(kraken_array_list_t* list, const void* element) {
    KRAKEN_CHECK_PTR(list, KRAKEN_ERR_INVALID_ARG, "Invalid array list pointer");
    KRAKEN_CHECK_PTR(element, KRAKEN_ERR_INVALID_ARG, "Invalid element pointer");

    const size_t current_size = list->size;
    const size_t new_size = current_size + 1;
    const size_t element_size = list->element_size;
    if(new_size > list->capacity) {// We need to make more space, over-allocate double the new size
        const size_t new_capacity = new_size << 1;
        const size_t new_capacity_bytes = new_capacity * element_size;
        list->memory = kraken_realloc(list->memory, new_capacity_bytes);
        list->capacity = new_capacity;
    }
    const size_t dest_offset = current_size * element_size;
    uint8_t* dest_address = (uint8_t*) list->memory + dest_offset;

    memcpy(dest_address, element, element_size);
    list->size = new_size;

    return KRAKEN_OK;
}

kraken_error_t kraken_array_list_remove(kraken_array_list_t* list, const void* element) {
    KRAKEN_CHECK_PTR(list, KRAKEN_ERR_INVALID_ARG, "Invalid array list pointer");
    KRAKEN_CHECK_PTR(element, KRAKEN_ERR_INVALID_ARG, "Invalid element pointer");

    const size_t element_size = list->element_size;
    const size_t current_size = list->size;
    for(size_t i = 0; i < current_size; i++) {
        const size_t current_offset = i * element_size;
        uint8_t* current_element = (uint8_t*) list->memory + current_offset;
        if(memcmp(current_element, element, element_size) != 0) {
            continue;
        }
        // We simply copy everything after the element to the address of the element
        const size_t elements_after = current_size - i;
        const size_t size_after = elements_after * element_size;
        const uint8_t* source_element = current_element + element_size;

        memcpy(current_element, source_element, size_after);
        list->size--;
        return KRAKEN_OK;// Found
    }

    return KRAKEN_ERR_INVALID_OP;// Not found
}

kraken_error_t kraken_array_list_get(kraken_array_list_t* list, const size_t index, void* element) {
    KRAKEN_CHECK_PTR(list, KRAKEN_ERR_INVALID_ARG, "Invalid array list pointer");
    KRAKEN_CHECK_PTR(element, KRAKEN_ERR_INVALID_ARG, "Invalid element pointer");

    KRAKEN_CHECK(index < list->size, KRAKEN_ERR_INVALID_ARG, "Array list index out of bounds");
    const size_t element_size = list->element_size;
    const size_t source_offset = index * element_size;
    const uint8_t* source_element = (const uint8_t*) list->memory + source_offset;
    memcpy(element, source_element, element_size);

    return KRAKEN_OK;
}

kraken_error_t kraken_array_list_index_of(kraken_array_list_t* list, const void* element, size_t* index) {
    KRAKEN_CHECK_PTR(list, KRAKEN_ERR_INVALID_ARG, "Invalid array list pointer");
    KRAKEN_CHECK_PTR(element, KRAKEN_ERR_INVALID_ARG, "Invalid element pointer");
    KRAKEN_CHECK_PTR(index, KRAKEN_ERR_INVALID_ARG, "Invalid index pointer");

    const size_t element_size = list->element_size;
    for(size_t i = 0; i < list->size; i++) {
        const size_t current_offset = i * element_size;
        const uint8_t* current_element = (const uint8_t*) list->memory + current_offset;
        if(memcmp(current_element, element, element_size) != 0) {
            continue;
        }
        *index = i;
        return KRAKEN_OK;// Found
    }

    return KRAKEN_ERR_INVALID_OP;// Not found
}

kraken_error_t kraken_array_list_set(kraken_array_list_t* list, const size_t index, const void* element) {
    KRAKEN_CHECK_PTR(list, KRAKEN_ERR_INVALID_ARG, "Invalid array list pointer");
    KRAKEN_CHECK_PTR(element, KRAKEN_ERR_INVALID_ARG, "Invalid element pointer");

    KRAKEN_CHECK(index < list->size, KRAKEN_ERR_INVALID_ARG, "Array list index out of bounds");
    const size_t element_size = list->element_size;
    const size_t dest_offset = index * element_size;
    uint8_t* dest_element = (uint8_t*) list->memory + dest_offset;
    memcpy(dest_element, element, element_size);

    return KRAKEN_OK;
}

kraken_error_t kraken_array_list_contains(kraken_array_list_t* list, const void* element, bool* contains) {
    KRAKEN_CHECK_PTR(list, KRAKEN_ERR_INVALID_ARG, "Invalid array list pointer");
    KRAKEN_CHECK_PTR(element, KRAKEN_ERR_INVALID_ARG, "Invalid element pointer");
    KRAKEN_CHECK_PTR(contains, KRAKEN_ERR_INVALID_ARG, "Invalid contains flag pointer");

    const size_t element_size = list->element_size;
    for(size_t i = 0; i < list->size; i++) {
        const size_t current_offset = i * element_size;
        const uint8_t* current_element = (const uint8_t*) list->memory + current_offset;
        if(memcmp(current_element, element, element_size) != 0) {
            continue;
        }
        *contains = true;
        return KRAKEN_OK;
    }
    *contains = false;
    return KRAKEN_OK;
}

kraken_error_t kraken_array_list_clear(kraken_array_list_t* list) {
    KRAKEN_CHECK_PTR(list, KRAKEN_ERR_INVALID_ARG, "Invalid array list pointer");
    list->size = 0;
    return KRAKEN_OK;
}

kraken_error_t kraken_array_list_destroy(kraken_array_list_t* list) {
    KRAKEN_CHECK_PTR(list, KRAKEN_ERR_INVALID_ARG, "Invalid array list pointer");
    kraken_free(list->memory);
    return KRAKEN_OK;
}

kraken_error_t kraken_array_list_free(kraken_array_list_t* list) {
    KRAKEN_CHECK_PTR(list, KRAKEN_ERR_INVALID_ARG, "Invalid array list pointer");
    kraken_array_list_destroy(list);
    kraken_free(list);
    return KRAKEN_OK;
}