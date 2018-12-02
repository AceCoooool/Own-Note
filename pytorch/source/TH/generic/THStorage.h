#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "generic/THStorage.h"
#else

/* on pourrait avoir un liste chainee
   qui initialise math, lab structures (or more).
   mouais -- complique.

   Pb: THMapStorage is kind of a class
   THLab_()... comment je m'en sors?

   en template, faudrait que je les instancie toutes!!! oh boy!
   Et comment je sais que c'est pour Cuda? Le type float est le meme dans les <>

   au bout du compte, ca serait sur des pointeurs float/double... etc... = facile.
   primitives??
 */

// 默认0111 --- 即除了view, 其他都是1
#define TH_STORAGE_REFCOUNTED 1  // 是否允许引用计数
#define TH_STORAGE_RESIZABLE  2  // 是否允许resize (Tensor的角度)
#define TH_STORAGE_FREEMEM    4  // 是否允许释放空间
#define TH_STORAGE_VIEW       8  // 是否运行view?

typedef struct THStorage
{
    real *data;       // 指向raw data
    ptrdiff_t size;   // 数据空间包含的元素数目
    int refcount;     // 引用计数
    char flag;        // 默认7=0111---用来表示运行哪些"操作"
    THAllocator *allocator;   // 内存管理器
    void *allocatorContext;   // 内容管理器:和文件操作有关
    struct THStorage *view;
} THStorage;

/* ---------------------返回成员对象--------------------- */
// 返回data
TH_API real* THStorage_(data)(const THStorage*);
// 返回size
TH_API ptrdiff_t THStorage_(size)(const THStorage*);
// 返回数据类型占据的字节数 --- 比如int为4
TH_API size_t THStorage_(elementSize)(void);

/* slow access -- checks everything */
TH_API void THStorage_(set)(THStorage*, ptrdiff_t, real);
TH_API real THStorage_(get)(const THStorage*, ptrdiff_t);

/* ---------------------新建storage对象--------------------- */
TH_API THStorage* THStorage_(new)(void);
TH_API THStorage* THStorage_(newWithSize)(ptrdiff_t size);
TH_API THStorage* THStorage_(newWithSize1)(real);
TH_API THStorage* THStorage_(newWithSize2)(real, real);
TH_API THStorage* THStorage_(newWithSize3)(real, real, real);
TH_API THStorage* THStorage_(newWithSize4)(real, real, real, real);
TH_API THStorage* THStorage_(newWithMapping)(const char *filename, ptrdiff_t size, int flags);

/* takes ownership of data */
TH_API THStorage* THStorage_(newWithData)(real *data, ptrdiff_t size);

// 新建一个storage, 令s->data指向新开辟的size大小的空间, s->size=size(并将其他参数按照默认设定)
TH_API THStorage* THStorage_(newWithAllocator)(ptrdiff_t size,
                                               THAllocator* allocator,
                                               void *allocatorContext);
// 新建一个storage, 并让s->data与给定的data指向同一片空间, s->size=size (其他按照默认设置)
TH_API THStorage* THStorage_(newWithDataAndAllocator)(
    real* data, ptrdiff_t size, THAllocator* allocator, void *allocatorContext);


/* should not differ with API */
TH_API void THStorage_(setFlag)(THStorage *storage, const char flag);
TH_API void THStorage_(clearFlag)(THStorage *storage, const char flag);
TH_API void THStorage_(retain)(THStorage *storage);
TH_API void THStorage_(swap)(THStorage *storage1, THStorage *storage2);

/* might differ with other API (like CUDA) */
TH_API void THStorage_(free)(THStorage *storage);
TH_API void THStorage_(resize)(THStorage *storage, ptrdiff_t size);
TH_API void THStorage_(fill)(THStorage *storage, real value);

#endif
