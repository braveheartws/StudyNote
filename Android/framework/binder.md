## binder源码分析



### 一. binder驱动注册以及初始化

#### 1.1 常用结构体

```c
//binder驱动结构体
struct binder_device {
	struct hlist_node hlist;
	struct miscdevice miscdev;
	struct binder_context context;
};

//内核驱动和native交互api映射
static const struct file_operations binder_fops = {
	.owner = THIS_MODULE,
	.poll = binder_poll,
	.unlocked_ioctl = binder_ioctl,
	.compat_ioctl = binder_ioctl,
	.mmap = binder_mmap,
	.open = binder_open,
	.flush = binder_flush,
	.release = binder_release,
};

static struct vm_operations_struct binder_vm_ops = {
	.open = binder_vma_open,
	.close = binder_vma_close,
	.fault = binder_vm_fault,
};

struct binder_node {
	int debug_id;
	struct binder_work work;
	union {
		struct rb_node rb_node;
		struct hlist_node dead_node;
	};
	struct binder_proc *proc;
	struct hlist_head refs;
	int internal_strong_refs;
	int local_weak_refs;
	int local_strong_refs;
	binder_uintptr_t ptr;
	binder_uintptr_t cookie;
	unsigned has_strong_ref:1;
	unsigned pending_strong_ref:1;
	unsigned has_weak_ref:1;
	unsigned pending_weak_ref:1;
	unsigned has_async_transaction:1;
	unsigned accept_fds:1;
	unsigned min_priority:8;
	struct list_head async_todo;
};

struct binder_proc {
	struct hlist_node proc_node; //进程节点
	struct rb_root threads;	//binder_thread根节点
	struct rb_root nodes;	//binder_node根节点
	struct rb_root refs_by_desc;	
	struct rb_root refs_by_node;
	int pid;	//进程id
	struct vm_area_struct *vma;	//指向进程虚拟地址空间的指针
	struct mm_struct *vma_vm_mm;	//相应进程的内存结构体
	struct task_struct *tsk;	//相应进程task结构体
	struct files_struct *files;	//相应进程文件结构体
	struct hlist_node deferred_work_node;
	int deferred_work;
	void *buffer;	//内核空间起始地址,
	ptrdiff_t user_buffer_offset;	//内核空间地址与用户空间地址偏移量

	struct list_head buffers;	// 总buffer
	struct rb_root free_buffers;	//可用空闲buffer
	struct rb_root allocated_buffers;	//已分配的buffer
	size_t free_async_space;	//异步空闲可用内存大小

	struct page **pages;	//指向物理内存页的二级指针
	size_t buffer_size;	//映射的内核空间大小
	uint32_t buffer_free;	//可用总内存
	struct list_head todo;	//将要执行的任务
	wait_queue_head_t wait;	//等待队列
	struct binder_stats stats;
	struct list_head delivered_death;
	int max_threads;
	int requested_threads;
	int requested_threads_started;
	int ready_threads;
	long default_priority;
	struct dentry *debugfs_entry;
	struct binder_context *context;
};

struct binder_thread {
	struct binder_proc *proc;
	struct rb_node rb_node;
	int pid;
	int looper;
	struct binder_transaction *transaction_stack;
	struct list_head todo;
	uint32_t return_error; /* Write failed, return error code in read buf */
	uint32_t return_error2; /* Write failed, return error code in read */
		/* buffer. Used when sending a reply to a dead process that */
		/* we are also waiting on */
	wait_queue_head_t wait;
	struct binder_stats stats;
};
```

----

#### 1.2 binder_init

```c
//起始方法
device_initcall(binder_init);

static int __init binder_init(void) {
    int ret;
	char *device_name, *device_names;
	struct binder_device *device;
	struct hlist_node *node, *tmp;
	//创建单线程工作队列
	binder_deferred_workqueue = create_singlethread_workqueue("binder");
    //分配内存
    device_names = kzalloc(strlen(binder_devices_param) + 1, GFP_KERNEL);
    //对device_names赋值
    strcpy(device_names, binder_devices_param);
    //调用初始化
    ret = init_binder_device(device_name);
}

static int __init init_binder_device(const char *name)
{
	int ret;
	struct binder_device *binder_device;
	//给binder_device分配内存
	binder_device = kzalloc(sizeof(*binder_device), GFP_KERNEL);
	if (!binder_device)
		return -ENOMEM;
	//初始化binder_device
	//映射native和驱动的方法 
	binder_device->miscdev.fops = &binder_fops;
	binder_device->miscdev.minor = MISC_DYNAMIC_MINOR;
	//binder名称
	binder_device->miscdev.name = name;

	binder_device->context.binder_context_mgr_uid = -1;
	binder_device->context.name = name;
	//注册成为misc设备
	ret = misc_register(&binder_device->miscdev);
	if (ret < 0) {
		kfree(binder_device);
		return ret;
	}
	//将binder_device->hlist添加到binder_devices的表头
	hlist_add_head(&binder_device->hlist, &binder_devices);

	return ret;
}

```

#### 1.3 binder_open

```c
static int binder_open(struct inode *nodp, struct file *filp)
{
	struct binder_proc *proc;
	struct binder_device *binder_dev;

	//binder_proc 分配内存
	proc = kzalloc(sizeof(*proc), GFP_KERNEL);
	if (proc == NULL)
		return -ENOMEM;
	//将当前线程的task保存到binder进程的tsk
	get_task_struct(current->group_leader);
	proc->tsk = current->group_leader;
	INIT_LIST_HEAD(&proc->todo);	//初始化todo列表
	init_waitqueue_head(&proc->wait);	//初始化等待队列
	//获取进程优先级
	proc->default_priority = task_nice(current);
	
	proc->context = &binder_dev->context;

	binder_lock(__func__);

	binder_stats_created(BINDER_STAT_PROC);
    //将proc->proc_node 插入到 binder_procs头部
	hlist_add_head(&proc->proc_node, &binder_procs);
    //进程pid
	proc->pid = current->group_leader->pid;
	INIT_LIST_HEAD(&proc->delivered_death);
    //将这个binder_proc与filp关联起来，这样下次通过filp就能找 到这个proc了
	filp->private_data = proc;

	binder_unlock(__func__);

	return 0;
}
```

----

#### 1.4 binder_mmap

```c
static int binder_mmap(struct file *filp, struct vm_area_struct *vma)	//vma 进程虚拟内存  linux/mm_types.h 定义的结构
{
	int ret;
	struct vm_struct *area;	//内核虚拟内存结构体
	struct binder_proc *proc = filp->private_data;	//binder_open里面跟file关联的结构体
	const char *failure_string;
	struct binder_buffer *buffer;
	//最大支持4M
	if ((vma->vm_end - vma->vm_start) > SZ_4M)
		vma->vm_end = vma->vm_start + SZ_4M;

	mutex_lock(&binder_mmap_lock);
	if (proc->buffer) {
		ret = -EBUSY;
		failure_string = "already mapped";
		goto err_already_mapped;
	}
	//采用 VM_IOREMAP方式，分配一个连续的内核虚拟内存，与进程虚拟内存大小一致
	area = get_vm_area(vma->vm_end - vma->vm_start, VM_IOREMAP);	//vmalloc.c
	
	//将proc中的buffer指针指向这块内核的虚拟内存
	proc->buffer = area->addr;
	// 计算出用户空间和内核空间的地址偏移量。地址偏移量 = 用户虚拟内存地址 - 内核虚拟内存地址
	proc->user_buffer_offset = vma->vm_start - (uintptr_t)proc->buffer;
	mutex_unlock(&binder_mmap_lock);
	//分配物理页的指针数组，数组大小为vma的等效page个数
	proc->pages = kzalloc(sizeof(proc->pages[0]) * ((vma->vm_end - vma->vm_start) / PAGE_SIZE), GFP_KERNEL);
	//映射的空间大小
	proc->buffer_size = vma->vm_end - vma->vm_start;

	vma->vm_ops = &binder_vm_ops;
	vma->vm_private_data = proc; // 内核虚拟空间的 信息 指向了binder驱动打开的proc信息
	//分配物理页面，同时映射到内核空间和进程空间，先分配1个物理页。
	if (binder_update_page_range(proc, 1, proc->buffer, proc->buffer + PAGE_SIZE, vma)) {
	}
	buffer = proc->buffer;	//buffer = 内核空间地址
	INIT_LIST_HEAD(&proc->buffers);	//初始化buffers链表
	list_add(&buffer->entry, &proc->buffers);	//将内核虚拟内存的entry节点 添加到 buffers头部
	buffer->free = 1;	//内核空间地址标记为可用,
	binder_insert_free_buffer(proc, buffer);	//插入到proc->free_buffers
	proc->free_async_space = proc->buffer_size / 2;//异步 1/2
	barrier();
	proc->files = get_files_struct(current);
	proc->vma = vma;	//proc.vma指向进程虚拟地址空间的指针
	proc->vma_vm_mm = vma->vm_mm; //指向进程内存地址空间

	return ret;
}

static int binder_update_page_range(struct binder_proc *proc, int allocate,
				    void *start, void *end,
				    struct vm_area_struct *vma)
{
	void *page_addr;
	unsigned long user_page_addr;
	struct vm_struct tmp_area;	//内核虚拟地址
	struct page **page;
	struct mm_struct *mm;

	if (allocate == 0)	//1:分配 0:释放
		goto free_range;

	//只执行一次循环
	for (page_addr = start; page_addr < end; page_addr += PAGE_SIZE) {
		int ret;
		struct page **page_array_ptr;
        //page_addr == start == proc->buffer  &proc->pages[0];
		page = &proc->pages[(page_addr - proc->buffer) / PAGE_SIZE];	// 0

		//分配1页物理内存
		*page = alloc_page(GFP_KERNEL | __GFP_HIGHMEM | __GFP_ZERO);
		
		tmp_area.addr = page_addr;
		tmp_area.size = PAGE_SIZE + PAGE_SIZE /* guard page? */;
		page_array_ptr = page;
		//将物理控件映射到内核虚拟空间
		ret = map_vm_area(&tmp_area, PAGE_KERNEL, &page_array_ptr);
		
		user_page_addr =
			(uintptr_t)page_addr + proc->user_buffer_offset;
		// 物理空间映射到虚拟进程空间
		ret = vm_insert_page(vma, user_page_addr, page[0]);
		
	}
	return 0;
}
```

----

#### 1.5 binder_ioctl

```c
static long binder_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
    switch (cmd) {
	case BINDER_WRITE_READ: {
		struct binder_write_read bwr;

		if (copy_from_user(&bwr, ubuf, sizeof(bwr))) {
			ret = -EFAULT;
			goto err;
		}
	

		if (bwr.write_size > 0) {
			ret = binder_thread_write(proc, thread, bwr.write_buffer, bwr.write_size, &bwr.write_consumed);
			trace_binder_write_done(ret);
			if (ret < 0) {
				bwr.read_consumed = 0;
				if (copy_to_user(ubuf, &bwr, sizeof(bwr)))
					ret = -EFAULT;
				goto err;
			}
		}
		if (bwr.read_size > 0) {
			ret = binder_thread_read(proc, thread, bwr.read_buffer, bwr.read_size, &bwr.read_consumed, filp->f_flags & O_NONBLOCK);
			trace_binder_read_done(ret);
			if (!list_empty(&proc->todo))
				wake_up_interruptible(&proc->wait);
			if (ret < 0) {
				if (copy_to_user(ubuf, &bwr, sizeof(bwr)))
					ret = -EFAULT;
				goto err;
			}
		}
		if (copy_to_user(ubuf, &bwr, sizeof(bwr))) {
			ret = -EFAULT;
			goto err;
		}
		break;
	}
	case BINDER_SET_MAX_THREADS:
		if (copy_from_user(&proc->max_threads, ubuf, sizeof(proc->max_threads))) {
			ret = -EINVAL;
			goto err;
		}
		break;
	case BINDER_SET_CONTEXT_MGR:
		if (context->binder_context_mgr_node) {
			pr_err("BINDER_SET_CONTEXT_MGR already set\n");
			ret = -EBUSY;
			goto err;
		}
		ret = security_binder_set_context_mgr(proc->tsk);
		if (ret < 0)
			goto err;
		if (context->binder_context_mgr_uid != -1) {
			if (context->binder_context_mgr_uid != current->cred->euid) {
				pr_err("binder: BINDER_SET_CONTEXT_MGR bad uid %d != %d\n",
				       current->cred->euid,
				       context->binder_context_mgr_uid);
				ret = -EPERM;
				goto err;
			}
		} else
			context->binder_context_mgr_uid = current->cred->euid;
		context->binder_context_mgr_node = binder_new_node(proc, 0, 0);
		if (!context->binder_context_mgr_node) {
			ret = -ENOMEM;
			goto err;
		}
		context->binder_context_mgr_node->local_weak_refs++;
		context->binder_context_mgr_node->local_strong_refs++;
		context->binder_context_mgr_node->has_strong_ref = 1;
		context->binder_context_mgr_node->has_weak_ref = 1;
		break;
        .....
}
```



### 二. ServiceManager注册

#### 2.1 结构体

```c
struct binder_state
{
    int fd;	//binder_open 返回的文件描述符
    void *mapped;
    size_t mapsize;
};
```



#### 2.2 打开驱动

```c
//service_manager.c
int main(int argc, char** argv)
{
    struct binder_state *bs;
    union selinux_callback cb;
    char *driver;

    if (argc > 1) {
        driver = argv[1];
    } else {
        driver = "/dev/binder";
    }

    bs = binder_open(driver, 128*1024);			//--> binder.c#open(driver,flag)

    if (binder_become_context_manager(bs)) {
        ALOGE("cannot become context manager (%s)\n", strerror(errno));
        return -1;
    }

    binder_loop(bs, svcmgr_handler);

    return 0;
}
```



##### 2.2.1 binder_open方法

```c

//frameworks\native\cmds\servicemanager\binder.c
struct binder_state *binder_open(const char* driver, size_t mapsize)	//drive = /dev/binder 
{
    struct binder_state *bs;
    struct binder_version vers;

    bs = malloc(sizeof(*bs));
    if (!bs) {
        errno = ENOMEM;
        return NULL;
    }

    bs->fd = open(driver, O_RDWR | O_CLOEXEC);
   
    bs->mapsize = mapsize;	//128k
    //建立内存映射进程虚拟内存与内核虚拟内存的映射,会分配物理页
    bs->mapped = mmap(NULL, mapsize, PROT_READ, MAP_PRIVATE, bs->fd, 0);
    
    return bs;
    return NULL;
}
```

##### 2.2.2 binder_become_context_manager

```c
int binder_become_context_manager(struct binder_state *bs)
{
    return ioctl(bs->fd, BINDER_SET_CONTEXT_MGR, 0);
}

//android_kernel_google_msm-lineage-18.1\drivers\staging\android\binder.c
static long binder_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    case BINDER_SET_CONTEXT_MGR:
		if (context->binder_context_mgr_node) {
			pr_err("BINDER_SET_CONTEXT_MGR already set\n");
			ret = -EBUSY;
			goto err;
		}
		ret = security_binder_set_context_mgr(proc->tsk);
		if (ret < 0)
			goto err;
		if (context->binder_context_mgr_uid != -1) {
			if (context->binder_context_mgr_uid != current->cred->euid) {
				pr_err("binder: BINDER_SET_CONTEXT_MGR bad uid %d != %d\n",
				       current->cred->euid,
				       context->binder_context_mgr_uid);
				ret = -EPERM;
				goto err;
			}
		} else
			context->binder_context_mgr_uid = current->cred->euid;
    	//创建新的binder_node节点.并将其设置为binder驱动的大管家
		context->binder_context_mgr_node = binder_new_node(proc, 0, 0);
		if (!context->binder_context_mgr_node) {
			ret = -ENOMEM;
			goto err;
		}
   		//binder的强弱引用操作
		context->binder_context_mgr_node->local_weak_refs++;
		context->binder_context_mgr_node->local_strong_refs++;
		context->binder_context_mgr_node->has_strong_ref = 1;
		context->binder_context_mgr_node->has_weak_ref = 1;
		break;
}

static struct binder_node *binder_new_node(struct binder_proc *proc,
					   binder_uintptr_t ptr,	//0
					   binder_uintptr_t cookie)	//0
{
	struct rb_node **p = &proc->nodes.rb_node;
	struct rb_node *parent = NULL;
	struct binder_node *node;

	while (*p) {	//链表的节点只定义了未赋值,它的data为0,指针为null,这里不会进入它的data为0
		parent = *p;
		node = rb_entry(parent, struct binder_node, rb_node);
		if (ptr < node->ptr)
			p = &(*p)->rb_left;
		else if (ptr > node->ptr)
			p = &(*p)->rb_right;
		else
			return NULL;
	}

	node = kzalloc(sizeof(*node), GFP_KERNEL);	//创建节点
	if (node == NULL)
		return NULL;
	binder_stats_created(BINDER_STAT_NODE);
	rb_link_node(&node->rb_node, parent, p);
	rb_insert_color(&node->rb_node, &proc->nodes);	//红黑树操作+赋值
	node->debug_id = ++binder_last_id;
	node->proc = proc;
	node->ptr = ptr;
	node->cookie = cookie;
	node->work.type = BINDER_WORK_NODE;
	INIT_LIST_HEAD(&node->work.entry);
	INIT_LIST_HEAD(&node->async_todo);
	return node;
}
```

##### 2.2.3 binder_loop(bs, svcmgr_handler)开启循环

```c
//frameworks\native\cmds\servicemanager\binder.c
void binder_loop(struct binder_state *bs, binder_handler func)
{
    int res;
    struct binder_write_read bwr;
    uint32_t readbuf[32];

    bwr.write_size = 0;
    bwr.write_consumed = 0;
    bwr.write_buffer = 0;

    readbuf[0] = BC_ENTER_LOOPER;
    binder_write(bs, readbuf, sizeof(uint32_t)); //1

    for (;;) {
        bwr.read_size = sizeof(readbuf);
        bwr.read_consumed = 0;
        bwr.read_buffer = (uintptr_t) readbuf;
        
        res = ioctl(bs->fd, BINDER_WRITE_READ, &bwr);//2
        
        res = binder_parse(bs, 0, (uintptr_t) readbuf, bwr.read_consumed, func);
        
    }
}
//1处
int binder_write(struct binder_state *bs, void *data, size_t len)
{
    struct binder_write_read bwr;
    int res;

    bwr.write_size = len;
    bwr.write_consumed = 0;
    bwr.write_buffer = (uintptr_t) data;//binder_loop// readbuf[0] = BC_ENTER_LOOPER;
    bwr.read_size = 0;
    bwr.read_consumed = 0;
    bwr.read_buffer = 0;
    res = ioctl(bs->fd, BINDER_WRITE_READ, &bwr);
    if (res < 0) {
        fprintf(stderr,"binder_write: ioctl failed (%s)\n",
                strerror(errno));
    }
    return res;
}

res = ioctl(bs->fd, BINDER_WRITE_READ, &bwr);
//最终调用binder.c
static int binder_thread_write(struct binder_proc *proc,
			struct binder_thread *thread,
			binder_uintptr_t binder_buffer, size_t size,
			binder_size_t *consumed){
    uint32_t cmd;
    switch (cmd) {
		...
        case BC_ENTER_LOOPER:
            binder_debug(BINDER_DEBUG_THREADS,
                         "%d:%d BC_ENTER_LOOPER\n",
                         proc->pid, thread->pid);
            if (thread->looper & BINDER_LOOPER_STATE_REGISTERED) {
                thread->looper |= BINDER_LOOPER_STATE_INVALID;
                binder_user_error("%d:%d ERROR: BC_ENTER_LOOPER called after BC_REGISTER_LOOPER\n",
                                  proc->pid, thread->pid);
            }
            thread->looper |= BINDER_LOOPER_STATE_ENTERED;	//设置了线程的状态
         ...
}
    
//2处 会进入binder_thread_read
    int binder_thread_read(struct binder_proc *proc,
			      struct binder_thread *thread,
			      binder_uintptr_t binder_buffer, size_t size,
			      binder_size_t *consumed, int non_block)
{
	void __user *buffer = (void __user *)(uintptr_t)binder_buffer;
	void __user *ptr = buffer + *consumed;
	void __user *end = buffer + size;

	int ret = 0;
	int wait_for_proc_work;

	if (*consumed == 0) {
		if (put_user(BR_NOOP, (uint32_t __user *)ptr))
			return -EFAULT;
		ptr += sizeof(uint32_t);
	}

	retry:
	wait_for_proc_work = thread->transaction_stack == NULL &&
				list_empty(&thread->todo);

	....
	if (wait_for_proc_work) {//true
		if (!(thread->looper & (BINDER_LOOPER_STATE_REGISTERED |
					BINDER_LOOPER_STATE_ENTERED))) {
				proc->pid, thread->pid, thread->looper);
			wait_event_interruptible(binder_user_error_wait,
						 binder_stop_on_user_error < 2);
		}
		binder_set_nice(proc->default_priority);
		if (non_block) {//false 阻塞的
			if (!binder_has_proc_work(proc, thread))
				ret = -EAGAIN;
		} else
            //等待
			ret = wait_event_freezable_exclusive(proc->wait, binder_has_proc_work(proc, thread));
	} else {
		if (non_block) {
			if (!binder_has_thread_work(thread))
				ret = -EAGAIN;
		} else
			ret = wait_event_freezable(thread->wait, binder_has_thread_work(thread));
	}
    


//这个方法很重要 addSercive getService
int svcmgr_handler(struct binder_state *bs,
                   struct binder_transaction_data *txn,
                   struct binder_io *msg,
                   struct binder_io *reply)
{
    struct svcinfo *si;
    uint16_t *s;
    size_t len;
    uint32_t handle;
    uint32_t strict_policy;
    int allow_isolated;

   
    strict_policy = bio_get_uint32(msg);
    s = bio_get_string16(msg, &len);
    if (s == NULL) {
        return -1;
    }

    if (sehandle && selinux_status_updated() > 0) {
        struct selabel_handle *tmp_sehandle = selinux_android_service_context_handle();
        if (tmp_sehandle) {
            selabel_close(sehandle);
            sehandle = tmp_sehandle;
        }
    }

    switch(txn->code) {
    case SVC_MGR_GET_SERVICE:
    case SVC_MGR_CHECK_SERVICE:
        s = bio_get_string16(msg, &len);
        if (s == NULL) {
            return -1;
        }
        handle = do_find_service(s, len, txn->sender_euid, txn->sender_pid);
        if (!handle)
            break;
        bio_put_ref(reply, handle);
        return 0;

    case SVC_MGR_ADD_SERVICE:
        s = bio_get_string16(msg, &len);
        
        handle = bio_get_ref(msg);
        allow_isolated = bio_get_uint32(msg) ? 1 : 0;
        if (do_add_service(bs, s, len, handle, txn->sender_euid,
            allow_isolated, txn->sender_pid))
            return -1;
        break;

    case SVC_MGR_LIST_SERVICES: {
        uint32_t n = bio_get_uint32(msg);
        si = svclist;
        while ((n-- > 0) && si)
            si = si->next;
        if (si) {
            bio_put_string16(reply, si->name);
            return 0;
        }
        return -1;
    }
    default:
        return -1;
    }

    bio_put_uint32(reply, 0);
    return 0;
}
```

#### 2.3 native获取sm

```c
//C:\workspace\code\aosp\android-8.0.0_r1\frameworks\native\libs\binder\IServiceManager.cpp
sp<IServiceManager> defaultServiceManager()
{
    if (gDefaultServiceManager != NULL) return gDefaultServiceManager;

    {
        AutoMutex _l(gDefaultServiceManagerLock);
        while (gDefaultServiceManager == NULL) {
            gDefaultServiceManager = interface_cast<IServiceManager>(
                ProcessState::self()->getContextObject(NULL));// 1
            if (gDefaultServiceManager == NULL)
                sleep(1);
        }
    }

    return gDefaultServiceManager;	//== BpServiceManager(BpBinder)
}

//1.处
interface_cast<IServiceManager>(ProcessState::self()->getContextObject(NULL));

//ProcessState::self()->getContextObject(NULL) 最终返回 BpBinder(0)

//C:\workspace\code\aosp\android-8.0.0_r1\frameworks\native\libs\binder\ProcessState.cpp
sp<ProcessState> ProcessState::self()
{
    Mutex::Autolock _l(gProcessMutex);
    if (gProcess != NULL) {
        return gProcess;
    }
    gProcess = new ProcessState("/dev/binder");//1.1
    return gProcess;
}
//1.1ProcessState初始化
ProcessState::ProcessState(const char *driver)
    : mDriverName(String8(driver))
    , mDriverFD(open_driver(driver))//1.1.1
    , mVMStart(MAP_FAILED)
    , mThreadCountLock(PTHREAD_MUTEX_INITIALIZER)
    , mThreadCountDecrement(PTHREAD_COND_INITIALIZER)
    , mExecutingThreadsCount(0)
    , mMaxThreads(DEFAULT_MAX_BINDER_THREADS)
    , mStarvationStartTimeMs(0)
    , mManagesContexts(false)
    , mBinderContextCheckFunc(NULL)
    , mBinderContextUserData(NULL)
    , mThreadPoolStarted(false)
    , mThreadPoolSeq(1)
{
    if (mDriverFD >= 0) {
        //#define BINDER_VM_SIZE ((1 * 1024 * 1024) - sysconf(_SC_PAGE_SIZE) * 2)
        // mmap the binder, providing a chunk of virtual address space to receive transactions.
        mVMStart = mmap(0, BINDER_VM_SIZE, PROT_READ, MAP_PRIVATE | MAP_NORESERVE, mDriverFD, 0);
    }
}

//1.1.1
static int open_driver(const char *driver)
{
    int fd = open(driver, O_RDWR | O_CLOEXEC);
    if (fd >= 0) {
        int vers = 0;
        status_t result = ioctl(fd, BINDER_VERSION, &vers);
        if (result == -1) {
        }
        size_t maxThreads = DEFAULT_MAX_BINDER_THREADS;
        result = ioctl(fd, BINDER_SET_MAX_THREADS, &maxThreads);	//1.1.1.1
    return fd;
}
//1.1.1.1
case BINDER_SET_MAX_THREADS:
    //设置最大线程数
    if (copy_from_user(&proc->max_threads, ubuf, sizeof(proc->max_threads))) {
    }
break;

//getContextObject();
 sp<IBinder> ProcessState::getContextObject(const sp<IBinder>& /*caller*/)
{
    return getStrongProxyForHandle(0);
}
    
//sp<IBinder> ProcessState::getStrongProxyForHandle(int32_t handle)
sp<IBinder> ProcessState::getStrongProxyForHandle(int32_t handle)
{
    sp<IBinder> result;
    b = new BpBinder(handle); 
    result = b;
    return result;
}
    
//C:\workspace\code\aosp\android-8.0.0_r1\frameworks\native\libs\binder\include\binder\IInterface.h
template<typename INTERFACE>
inline sp<INTERFACE> interface_cast(const sp<IBinder>& obj)
{
    return INTERFACE::asInterface(obj);
}

#define DECLARE_META_INTERFACE(INTERFACE)                               \
static const ::android::String16 descriptor;                        \
static ::android::sp<I##INTERFACE> asInterface(                     \
    const ::android::sp<::android::IBinder>& obj);              \
virtual const ::android::String16& getInterfaceDescriptor() const;  \
I##INTERFACE();                                                     \
virtual ~I##INTERFACE();                                            \

#define IMPLEMENT_META_INTERFACE(INTERFACE, NAME)                       \
    const ::android::String16 I##INTERFACE::descriptor(NAME);           \
    const ::android::String16&                                          \
            I##INTERFACE::getInterfaceDescriptor() const {              \
        return I##INTERFACE::descriptor;                                \
    }                                                                   \
    ::android::sp<I##INTERFACE> I##INTERFACE::asInterface(              \
            const ::android::sp<::android::IBinder>& obj)               \
    {                                                                   \
        ::android::sp<I##INTERFACE> intr;                               \
        if (obj != NULL) {                                              \
            intr = static_cast<I##INTERFACE*>(                          \
                obj->queryLocalInterface(                               \
                        I##INTERFACE::descriptor).get());               \
            if (intr == NULL) {
    		//new BpServiceManager(BpBinder)
                intr = new Bp##INTERFACE(obj);                          \
            }                                                           \
        }                                                               \
        return intr;                                                    \
    }                                                                   \
    I##INTERFACE::I##INTERFACE() { }                                    \
	I##INTERFACE::~I##INTERFACE() { }                                   \

//new BpServiceManager:BpRefBase
  	mRemote = BpBinder

```

### 三 AMS添加到SM流程

#### 3.1 SystemServer创建

```java
//C:\workspace\code\aosp\android-8.0.0_r1\frameworks\base\services\core\java\com\android\server\SystemService.java
//1.开启服务
startBootstrapServices(t);
2.创建ATMS服务 AMS服务
ActivityTaskManagerService atm = mSystemServiceManager.startService(
                ActivityTaskManagerService.Lifecycle.class).getService();
        mActivityManagerService = ActivityManagerService.Lifecycle.startService(
                mSystemServiceManager, atm);
3.反射创建LifeCycle(mService = new ActivityManagerService(context, sAtm))

4.AMS.setSystemProcess
  ServiceManager.addService(Context.ACTIVITY_SERVICE, this, /* allowIsolated= */ true,
                    DUMP_FLAG_PRIORITY_CRITICAL | DUMP_FLAG_PRIORITY_NORMAL | DUMP_FLAG_PROTO);
5.SM.addService()
    getIServiceManager().addService(name, service, false);

6 获取IServiceManager
private static IServiceManager getIServiceManager() {
    sServiceManager = ServiceManagerNative
                .asInterface(Binder.allowBlocking(BinderInternal.getContextObject()));
    return sServiceManager;//new ServiceManagerProxy(obj);
}

6.1 BinderInternal.getContextObject()

6.1.1 //android_util_Bnider.cpp
static jobject android_os_BinderInternal_getContextObject(JNIEnv* env, jobject clazz)
{
    sp<IBinder> b = ProcessState::self()->getContextObject(NULL);
    return javaObjectForIBinder(env, b);	//把BpBinder 转换为java的对象
}

sp<IBinder> ProcessState::getContextObject(const sp<IBinder>& /*caller*/)
{
    return getStrongProxyForHandle(0);	//返回BpBinder(0)
}

7.ServiceManagerNative添加服务
    public void addService(String name, IBinder service, boolean allowIsolated)
            throws RemoteException {
        Parcel data = Parcel.obtain();
        Parcel reply = Parcel.obtain();
        data.writeInterfaceToken(IServiceManager.descriptor);
        data.writeString(name);
        data.writeStrongBinder(service);
        data.writeInt(allowIsolated ? 1 : 0);
        mRemote.transact(ADD_SERVICE_TRANSACTION, data, reply, 0);
        reply.recycle();
        data.recycle();
    }
8.BpBinder.transct
status_t BpBinder::transact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    // Once a binder has died, it will never come back to life.
    if (mAlive) {
        status_t status = IPCThreadState::self()->transact(
            mHandle, code, data, reply, flags);
        if (status == DEAD_OBJECT) mAlive = 0;
        return status;
    }

    return DEAD_OBJECT;
}

8.1
status_t IPCThreadState::transact(int32_t handle,
                                  uint32_t code, const Parcel& data,
                                  Parcel* reply, uint32_t flags)
{
    err = waitForResponse(reply);
}

8.1.1
status_t IPCThreadState::waitForResponse(Parcel *reply, status_t *acquireResult)
{
    uint32_t cmd;
    int32_t err;

    while (1) {
        talkWithDriver();	//8.1.1.1
        
    }    
}


```



