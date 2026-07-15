/*
** EPITECH PROJECT, 2026
** rush1
** File description:
** list
*/

#include <stdlib.h>
#include "list.h"
#include "G-PDG-300_rush1/new.h"
#include "G-PDG-300_rush1/raise.h"

typedef struct node_s node_t;
struct node_s {
    Object *value;
    node_t *next;
};

typedef struct {
    Container base;
    Class *_type;
    size_t _size;
    node_t *_head;
} class_list_t;

typedef struct {
    Iterator base;
    class_list_t *_list;
    node_t *_node;
} class_list_iterator_t;

static void list_iterator_constructor
(class_list_iterator_t *this, va_list *args)
{
    this->_list = va_arg(*args, class_list_t *);
    this->_node = va_arg(*args, node_t *);
}

static void list_iterator_increment(class_list_iterator_t *this)
{
    if (this->_node != NULL)
        this->_node = this->_node->next;
}

static Object *list_iterator_getvalue(class_list_iterator_t *this)
{
    if (this->_node == NULL)
        return (NULL);
    return (this->_node->value);
}

static bool list_iterator_is_equal(class_list_iterator_t *this,
    class_list_iterator_t *to_compare)
{
    return (this->_node == to_compare->_node);
}

static bool list_iterator_lt
(class_list_iterator_t *this, class_list_iterator_t *other)
{
    return (this->_node != other->_node);
}

static const class_list_iterator_t _iterator_descr = {
    {   /* Iterator struct */
        {   /* Class struct */
            .__size__ = sizeof(class_list_iterator_t),
            .__name__ = "ListIterator",
            .__ctor__ = (ctor_t)&list_iterator_constructor,
            .__dtor__ = NULL,
            .__str__ = NULL,
            .__add__ = NULL,
            .__sub__ = NULL,
            .__mul__ = NULL,
            .__div__ = NULL,
            .__eq__ = (binary_comparator_t)&list_iterator_is_equal,
            .__gt__ = NULL,
            .__lt__ = (binary_comparator_t)&list_iterator_lt
        },
        .__incr__ = (incr_t)&list_iterator_increment,
        .__getval__ = (getval_t)&list_iterator_getvalue,
        .__setval__ = NULL
    },
    ._list = NULL,
    ._node = NULL
};

static const Class *ListIterator = (const Class *)&_iterator_descr;

static void list_creator(class_list_t *this, va_list *args)
{
    va_list tmp;
    node_t **current;
    node_t *new_node;

    this->_size = va_arg(*args, size_t);
    this->_type = va_arg(*args, Class *);
    this->_head = NULL;
    current = &this->_head;
    for (size_t i = 0; i < this->_size; i++) {
        new_node = malloc(sizeof(node_t));
        if (new_node == NULL)
            raise("Out of memory");
        va_copy(tmp, *args);
        new_node->value = va_new(this->_type, &tmp);
        va_end(tmp);
        new_node->next = NULL;
        *current = new_node;
        current = &new_node->next;
    }
}

static void list_destructor(class_list_t *this)
{
    node_t *current = this->_head;
    node_t *next = NULL;

    while (current != NULL) {
        next = current->next;
        delete(current->value);
        free(current);
        current = next;
    }
}

static size_t list_len(class_list_t *this)
{
    return (this->_size);
}

static Iterator *list_begin(class_list_t *this)
{
    return (new(ListIterator, this, this->_head));
}

static Iterator *list_end(class_list_t *this)
{
    return (new(ListIterator, this, NULL));
}

static Object *list_getitem(class_list_t *this, size_t index)
{
    node_t *tmp = this->_head;

    if (index >= this->_size)
        return (NULL);
    for (size_t i = 0; i < index; i++)
        tmp = tmp->next;
    return (tmp->value);
}

static void list_setitem(class_list_t *this, size_t index, ...)
{
    node_t *tmp = this->_head;
    va_list ap;

    if (index >= this->_size || tmp == NULL)
        return;
    for (size_t i = 0; i < index; i++)
        tmp = tmp->next;
    delete(tmp->value);
    va_start(ap, index);
    tmp->value = va_new(this->_type, &ap);
    va_end(ap);
}

static Object *list_add(const void *this_void, const void *value_void)
{
    class_list_t *this = (class_list_t *)this_void;
    Object *value = (Object *)value_void;
    node_t *new_node = malloc(sizeof(node_t));
    node_t *current = this->_head;
    va_list tmp;

    if (new_node == NULL)
        raise("Out of memory");
    va_copy(tmp, (va_list){0});
    new_node->value = value;
    new_node->next = NULL;
    if (this->_head == NULL) {
        this->_head = new_node;
    } else {
        while (current->next != NULL)
            current = current->next;
        current->next = new_node;
    }
    this->_size++;
    return (Object *)this;
}



static const class_list_t _description = {
    {   /* Container struct */
        {   /* Class struct */
            .__size__ = sizeof(class_list_t),
            .__name__ = "List",
            .__ctor__ = (ctor_t)&list_creator,
            .__dtor__ = (dtor_t)&list_destructor,
            .__add__ = &list_add
        },
        .__len__ = (len_t)&list_len,
        .__begin__ = (iter_t)&list_begin,
        .__end__ = (iter_t)&list_end,
        .__getitem__ = (getitem_t)&list_getitem,
        .__setitem__ = (setitem_t)&list_setitem
    },
    ._type = NULL,
    ._size = 0,
    ._head = NULL
};

const Class *List = (const Class *)&_description;
