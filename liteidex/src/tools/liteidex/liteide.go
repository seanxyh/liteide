// liteide.go
package main

/*
extern int cdrv_main(int argc,char** argv);
extern void cdrv_init(void *fn);
extern void cdrv_cb(void *cb, void *id, void *reply, int size, int err, void* ctx);
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

func cdrv_cb(cb unsafe.Pointer, id []byte, reply []byte, err int, ctx unsafe.Pointer) {
	C.cdrv_cb(cb, unsafe.Pointer(&id[0]), unsafe.Pointer(&reply[0]), C.int(len(reply)), C.int(err), ctx)
}

//export godrv_call
func godrv_call(id unsafe.Pointer, id_size C.int, args unsafe.Pointer, size C.int, cb unsafe.Pointer, ctx unsafe.Pointer) int {
	return go_call(C.GoBytes(id, id_size), C.GoBytes(args, size), cb, ctx)
}

var (
	cmdFuncMap = make(map[string]func(args []byte) (error, []byte))
)

func RegCmd(id string, fn func(args []byte) (error, []byte)) {
	cmdFuncMap[id] = fn
}

func go_call(id []byte, args []byte, cb unsafe.Pointer, ctx unsafe.Pointer) int {
	if fn, ok := cmdFuncMap[string(id)]; ok {
		go func(_id, _args []byte, _cb, _ctx unsafe.Pointer) {
			err, rep := fn(id)
			if err != nil {
				cdrv_cb(_cb, _id, []byte{0}, -1, _ctx)
			}
			cdrv_cb(_cb, _id, append(rep, 0), 0, _ctx)
		}(id, args, cb, ctx)
		return 0
	}
	return -1
}

func cmdList(args []byte) (error, []byte) {
	var cmds []byte
	for cmd, _ := range cmdFuncMap {
		cmds = append(cmds, []byte(cmd+" ")...)
	}
	return nil, cmds
}

func init() {
	RegCmd("cmdlist", cmdList)
}
