// liteide.go
package main

/*
typedef void (*DRV_CALLBACK)(char *id, char *reply, int len, void* ctx);
extern int cdrv_main(int argc,char** argv);
extern void cdrv_init(void *fn);
extern void cdrv_cb(void *cb, void *id, void *reply, int size, void* ctx);
static void cdrv_init_ex()
{
	extern int godrv_call(void* id,int id_size, void* args, int args_size, void* cb, void* ctx);
	cdrv_init(&godrv_call);
}
#cgo LDFLAGS: -L../../../liteide/lib/liteide -lliteide
*/
import "C"
import "unsafe"

func liteide(args []string) int {
	argc := len(args)
	var cargs []*C.char
	for _, arg := range args {
		size := len(arg)
		data := make([]C.char, size+1)
		for i := 0; i < size; i++ {
			data[i] = (C.char)(arg[i])
		}
		data[size] = 0
		cargs = append(cargs, &data[0])
	}
	C.cdrv_init_ex()
	return int(C.cdrv_main(C.int(argc), &cargs[0]))
}

func cdrv_cb(cb unsafe.Pointer, id []byte, reply []byte, ctx unsafe.Pointer) {
	C.cdrv_cb(cb, unsafe.Pointer(&id[0]), unsafe.Pointer(&reply[0]), C.int(len(reply)), ctx)
}

//export godrv_call
func godrv_call(id unsafe.Pointer, id_size C.int, args unsafe.Pointer, size C.int, cb unsafe.Pointer, ctx unsafe.Pointer) int {
	return go_call(C.GoBytes(id, id_size), C.GoBytes(args, size), cb, ctx)
}

var (
	gocmd = make(map[string]func(args []byte) []byte)
)

func RegFunc(id string, fn func(args []byte) []byte) {
	gocmd[id] = fn
}

func go_call(id []byte, args []byte, cb unsafe.Pointer, ctx unsafe.Pointer) int {
	if fn, ok := gocmd[string(id)]; ok {
		go func(_id, _args []byte, _cb, _ctx unsafe.Pointer) {
			cdrv_cb(_cb, _id, append(fn(id), 0), _ctx)
		}(id, args, cb, ctx)
		return 0
	}
	return -1
}
