#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "generic/THStorage.c"
#else

/* ---------------------返回成员对象--------------------- */
// 返回data
real* THStorage_(data)(const THStorage *self)
{
  return self->data;
}

// 返回size
ptrdiff_t THStorage_(size)(const THStorage *self)
{
  return self->size;
}

// 返回数据类型占据的字节数 --- 比如int为4
size_t THStorage_(elementSize)()
{
  return sizeof(real);
}

/* ---------------------新建storage对象--------------------- */
// 新建一个storage, 令s->data指向新开辟的size大小的空间, s->size=size(并将其他参数按照默认设定)
THStorage* THStorage_(newWithAllocator)(ptrdiff_t size,
                                        THAllocator *allocator,
                                        void *allocatorContext)
{
  THStorage *storage = THAlloc(sizeof(THStorage));
  storage->data = allocator->malloc(allocatorContext, sizeof(real)*size);
  storage->size = size;
  storage->refcount = 1;
  storage->flag = TH_STORAGE_REFCOUNTED | TH_STORAGE_RESIZABLE | TH_STORAGE_FREEMEM;
  storage->allocator = allocator;
  storage->allocatorContext = allocatorContext;
  return storage;
}

// 新建一个storage, 并让s->data与给定的data指向同一片空间, s->size=size (其他按照默认设置)
THStorage* THStorage_(newWithDataAndAllocator)(real* data, ptrdiff_t size,
                                               THAllocator* allocator,
                                               void* allocatorContext) {
  THStorage *storage = THAlloc(sizeof(THStorage));
  storage->data = data;
  storage->size = size;
  storage->refcount = 1;
  storage->flag = TH_STORAGE_REFCOUNTED | TH_STORAGE_RESIZABLE | TH_STORAGE_FREEMEM;
  storage->allocator = allocator;
  storage->allocatorContext = allocatorContext;
  return storage;
}

// 采用默认分配器, 令s->data指向data同一片空间
THStorage* THStorage_(newWithData)(real *data, ptrdiff_t size)
{
  return THStorage_(newWithDataAndAllocator)(data, size,
                                             &THDefaultAllocator, NULL);
}

// 新建一个指向size大小空间
THStorage* THStorage_(newWithSize)(ptrdiff_t size)
{
  return THStorage_(newWithAllocator)(size, &THDefaultAllocator, NULL);
}

// 新建一个size=0的storage
THStorage* THStorage_(new)(void)
{
  return THStorage_(newWithSize)(0);
}

// 新建一个size=1的storage, 其s->data[0]=data0
THStorage* THStorage_(newWithSize1)(real data0)
{
  THStorage *self = THStorage_(newWithSize)(1);
  self->data[0] = data0;
  return self;
}

// 新建一个size=2的storage, 其s->data={data0, data1}
THStorage* THStorage_(newWithSize2)(real data0, real data1)
{
  THStorage *self = THStorage_(newWithSize)(2);
  self->data[0] = data0;
  self->data[1] = data1;
  return self;
}

// 新建一个size=3的storage, 其s->data={data0, data1, data2}
THStorage* THStorage_(newWithSize3)(real data0, real data1, real data2)
{
  THStorage *self = THStorage_(newWithSize)(3);
  self->data[0] = data0;
  self->data[1] = data1;
  self->data[2] = data2;
  return self;
}

// 新建一个size=3的storage, 其s->data={data0, data1, data2, data3}
THStorage* THStorage_(newWithSize4)(real data0, real data1, real data2, real data3)
{
  THStorage *self = THStorage_(newWithSize)(4);
  self->data[0] = data0;
  self->data[1] = data1;
  self->data[2] = data2;
  self->data[3] = data3;
  return self;
}

// TODO: 先略过
THStorage* THStorage_(newWithMapping)(const char *filename, ptrdiff_t size, int flags)
{
  THMapAllocatorContext *ctx = THMapAllocatorContext_new(filename, flags);

  THStorage *storage = THStorage_(newWithAllocator)(size,
                                                    &THMapAllocator,
                                                    ctx);

  if(size <= 0)
    storage->size = THMapAllocatorContext_size(ctx)/sizeof(real);

  THStorage_(clearFlag)(storage, TH_STORAGE_RESIZABLE);

  return storage;
}


/* ---------------------修改storage的flag--------------------- */
// 将flag中为1的位"加到" s->flag中
void THStorage_(setFlag)(THStorage *storage, const char flag)
{
  storage->flag |= flag;
}

// 将flag中为1的位 从s->flag中"删除"
void THStorage_(clearFlag)(THStorage *storage, const char flag)
{
  storage->flag &= ~flag;
}


/* ---------------------其他一些函数--------------------- */
// 增加对storage的引用计数 (前提是允许引用计数)
void THStorage_(retain)(THStorage *storage)
{
  if(storage && (storage->flag & TH_STORAGE_REFCOUNTED))
    THAtomicIncrementRef(&storage->refcount);
}

// 释放storage对象, 只有storage的引用计数为1时才会"真的"释放该空间
void THStorage_(free)(THStorage *storage)
{
  if(!storage)
    return;

  if((storage->flag & TH_STORAGE_REFCOUNTED) && (THAtomicGet(&storage->refcount) > 0))
  {
    if(THAtomicDecrementRef(&storage->refcount))
    {
      if(storage->flag & TH_STORAGE_FREEMEM) {
        storage->allocator->free(storage->allocatorContext, storage->data);
      }
      if(storage->flag & TH_STORAGE_VIEW) {
        THStorage_(free)(storage->view);
      }
      THFree(storage);
    }
  }
}

/* ---------------------对data进行的操作--------------------- */
// 将storage所占的内存空间调整为size
void THStorage_(resize)(THStorage *storage, ptrdiff_t size)
{
  if(storage->flag & TH_STORAGE_RESIZABLE)
  {
    if(storage->allocator->realloc == NULL) {
      /* case when the allocator does not have a realloc defined */
      real *old_data = storage->data;
      ptrdiff_t old_size = storage->size;
      if (size == 0) {
        storage->data = NULL;
      } else {
        storage->data = storage->allocator->malloc(
            storage->allocatorContext,
            sizeof(real)*size);
      }
      storage->size = size;
      if (old_data != NULL) {
        ptrdiff_t copy_size = old_size;
        if (storage->size < copy_size) {
          copy_size = storage->size;
        }
        if (copy_size > 0) {
          memcpy(storage->data, old_data, sizeof(real)*copy_size);
        }
        storage->allocator->free(storage->allocatorContext, old_data);
      }
    } else {
      storage->data = storage->allocator->realloc(
              storage->allocatorContext,
              storage->data,
              sizeof(real)*size);
      storage->size = size;
    }
  } else {
    THError("Trying to resize storage that is not resizable");
  }
}

// 将storage的data指向的空间的元素全改为value
void THStorage_(fill)(THStorage *storage, real value)
{
  ptrdiff_t i;
  for(i = 0; i < storage->size; i++)
    storage->data[i] = value;
}

// s->data[idx] = value
void THStorage_(set)(THStorage *self, ptrdiff_t idx, real value)
{
  THArgCheck((idx >= 0) && (idx < self->size), 2, "out of bounds");
  self->data[idx] = value;
}

// 返回s->data[idx]
real THStorage_(get)(const THStorage *self, ptrdiff_t idx)
{
  THArgCheck((idx >= 0) && (idx < self->size), 2, "out of bounds");
  return self->data[idx];
}

// 交换两个storage (除了引用计数外, 其他均进行交换)
void THStorage_(swap)(THStorage *storage1, THStorage *storage2)
{
#define SWAP(val) { val = storage1->val; storage1->val = storage2->val; storage2->val = val; }
    real *data;
    ptrdiff_t size;
    char flag;
    THAllocator *allocator;
    void *allocatorContext;
    struct THStorage *view;

    SWAP(data);
    SWAP(size);
    SWAP(flag);
    // don't swap refcount!
    SWAP(allocator);
    SWAP(allocatorContext);
    SWAP(view);
#undef SWAP
}

#endif
