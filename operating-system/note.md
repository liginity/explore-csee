### mmap 和 brk 两个系统调用的互相影响

- [./test-mmap-and-brk.c](./test-mmap-and-brk.c)

查看程序输出的地址值时，需要注意 ASLR，地址空间布局随机化。

mmap 可以覆盖 brk 已经占有的区间。
而 brk 检测到高地址处有其它映射时，会失败。

假设 brk 管理的地址范围是 `[end, brk_addr1]`。
那么 mmap 可以在这个区间内添加映射，需要使用 `MAP_FIXED` flag。
然后，继续假设已经通过 mmap 在 `[brk_addr1 + PAGE_SIZE, brk_addr1 + PAGE_SIZE * 2]` 处设置了内存映射。
那么，brk 将不能越过这个区间。
