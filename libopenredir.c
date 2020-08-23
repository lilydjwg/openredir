#include<stdarg.h>
#include<dlfcn.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<unistd.h>

#include<lua.h>
#include<lualib.h>
#include<lauxlib.h>




static int lib_initialized = 0;
static int (*orig_open)(const char*, int, mode_t) = 0;
static int (*orig_open64)(const char*, int, mode_t) = 0;
static int (*orig_creat)(const char*, mode_t) = 0;
static ssize_t (*orig_readlink)(const char*, char*, size_t) = 0;
static int (*orig___open_2)(const char*, int) = 0;
static int (*orig___open64_2)(const char*, int) = 0;
static int (*orig___lxstat)(int, const char*, void*) = 0;
static int (*orig___lxstat64)(int, const char*, void*) = 0;
static int (*orig_execve)(const char*, char *const*, char *const*) = 0;

static lua_State *L = NULL;
void lib_init();

void die(char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
  fflush(stderr);
  exit(-1);
}

static char* redirect(const char* func, const char* file){
  int ret;
  const char *new;
  lua_getglobal(L, "redirect");
  lua_pushstring(L, func);
  lua_pushstring(L, file);
  ret = lua_pcall(L, 2, 1, 0);
  if(ret){
    fprintf(stderr, "取得重定向文件路径时出错了： %s\n", lua_tostring(L, -1));
    lua_pop(L, 1); /* 错误信息 */
    return (char*)file;
  }else{
    new = lua_tostring(L, -1);
    lua_pop(L, 1); /* 返回值 */
  }
  return (char*)new;
}


int open(const char* file, int flags, mode_t mode) {
  lib_init();
  file = redirect("open", file);
  return orig_open(file, flags, mode);
}

int open64(const char* file, int flags, mode_t mode) {
  lib_init();
  file = redirect("open64", file);
  return orig_open64(file, flags, mode);
}

int creat(const char* file, mode_t mode) {
  lib_init();
  file = redirect("creat", file);
  return orig_creat(file, mode);
}

ssize_t readlink(const char* file, char* buf, size_t bufsiz) {
  lib_init();
  file = redirect("readlink", file);
  return orig_readlink(file, buf, bufsiz);
}

int __open_2(const char* file, int flags) {
  lib_init();
  file = redirect("__open_2", file);
  return orig___open_2(file, flags);
}

int __open64_2(const char* file, int flags) {
  lib_init();
  file = redirect("__open64_2", file);
  return orig___open64_2(file, flags);
}

int __lxstat(int vers, const char* file, void* buf) {
  lib_init();
  file = redirect("__lxstat", file);
  return orig___lxstat(vers, file, buf);
}

int __lxstat64(int vers, const char* file, void* buf) {
  lib_init();
  file = redirect("__lxstat64", file);
  return orig___lxstat64(vers, file, buf);
}

int execve(const char* file, char *const* argv, char *const* envp) {
  lib_init();
  file = redirect("execve", file);
  return orig_execve(file, argv, envp);
}


void lib_init() {
  void *libhdl;
  char *dlerr;

  if (lib_initialized) return;

  if (!(libhdl=dlopen("libc.so.6", RTLD_LAZY)))
    die("Failed to patch library calls: %s", dlerror());

  
  orig_open = dlsym(libhdl, "open");
  if ((dlerr=dlerror()) != NULL)
    die("Failed to patch open() library call: %s", dlerr);
  
  orig_open64 = dlsym(libhdl, "open64");
  if ((dlerr=dlerror()) != NULL)
    die("Failed to patch open64() library call: %s", dlerr);
  
  orig_creat = dlsym(libhdl, "creat");
  if ((dlerr=dlerror()) != NULL)
    die("Failed to patch creat() library call: %s", dlerr);
  
  orig_readlink = dlsym(libhdl, "readlink");
  if ((dlerr=dlerror()) != NULL)
    die("Failed to patch readlink() library call: %s", dlerr);
  
  orig___open_2 = dlsym(libhdl, "__open_2");
  if ((dlerr=dlerror()) != NULL)
    die("Failed to patch __open_2() library call: %s", dlerr);
  
  orig___open64_2 = dlsym(libhdl, "__open64_2");
  if ((dlerr=dlerror()) != NULL)
    die("Failed to patch __open64_2() library call: %s", dlerr);
  
  orig___lxstat = dlsym(libhdl, "__lxstat");
  if ((dlerr=dlerror()) != NULL)
    die("Failed to patch __lxstat() library call: %s", dlerr);
  
  orig___lxstat64 = dlsym(libhdl, "__lxstat64");
  if ((dlerr=dlerror()) != NULL)
    die("Failed to patch __lxstat64() library call: %s", dlerr);
  
  orig_execve = dlsym(libhdl, "execve");
  if ((dlerr=dlerror()) != NULL)
    die("Failed to patch execve() library call: %s", dlerr);
  

  int ret;
  L = luaL_newstate();
  luaL_openlibs(L);
  char config[PATH_MAX];
  if (getenv("OPENREDIR_CONFDIR"))
    strcpy(config, getenv("OPENREDIR_CONFDIR"));
  else
    strcpy(config, getenv("HOME"));

  strcat(config, "/.openredir.lua");
  ret = luaL_dofile(L, config);
  if(ret){
    die("Error run ~/.openredir.lua");
  }
  lua_getglobal(L, "redirect");
  if(!lua_isfunction(L,-1)){
    die("Error run 'redirect' function in openredir.lua");
  }
  lua_pop(L, 1);

  lib_initialized = 1;
}

/* vim: set syntax=c.tornadotmpl: */
