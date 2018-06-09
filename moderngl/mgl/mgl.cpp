#include "mgl.hpp"

#include "context.hpp"
#include "tools.hpp"

PyObject * module_error;

PyObject * tobytes_str;

#ifdef MGL_DEBUG
PyObject * moderngl_debugger;
#endif
PyObject * moderngl;
PyObject * numpy;
PyObject * pillow;

PyObject * numpy_frombuffer;

PyTypeObject * Context_class;
int Context_class_mglo;
int Context_class_framebuffers;
int Context_class_limits;
int Context_class_screen;
int Context_class_fbo;
int Context_class_extra;

PyTypeObject * Buffer_class;
int Buffer_class_mglo;
int Buffer_class_size;
int Buffer_class_extra;

PyTypeObject * MGLContext_type;

PyMethodDef mgl_methods[] = {
	{"create_context", (PyCFunction)meth_create_context, METH_VARARGS, 0},
	{0},
};

PyModuleDef mgl_def = {PyModuleDef_HEAD_INIT, "moderngl.mgl", 0, -1, mgl_methods, 0, 0, 0, 0};

void initialize_module() {
	static bool initialized = false;
	if (initialized) {
		return;
	}

	moderngl = PyImport_ImportModule("moderngl");
	if (!moderngl) {
		return;
	}

	#ifdef MGL_DEBUG
	moderngl_debugger = PyImport_ImportModule("moderngl_debugger");
	if (!moderngl_debugger) {
		PyErr_Clear();
	}
	#endif

	numpy = PyImport_ImportModule("numpy");
	if (!numpy) {
		PyErr_Clear();
	}

	pillow = PyImport_ImportModule("PIL.Image");
	if (!pillow) {
		PyErr_Clear();
	}

	if (numpy) {
		numpy_frombuffer = PyObject_GetAttrString(numpy, "frombuffer");
	}

	module_error = PyObject_GetAttrString(moderngl, "Error");

	tobytes_str = PyUnicode_FromString("tobytes");

	MGLContext_type = MGLContext_define();

	int Context_slots = 0;
	Context_class = detect_class(moderngl, "Context", Context_slots);
	Context_class_mglo = slot_offset(Context_class, "_Context__mglo", Context_slots);
	Context_class_framebuffers = slot_offset(Context_class, "_Context__framebuffers", Context_slots);
	Context_class_limits = slot_offset(Context_class, "limits", Context_slots);
	Context_class_screen = slot_offset(Context_class, "screen", Context_slots);
	Context_class_fbo = slot_offset(Context_class, "fbo", Context_slots);
	Context_class_extra = slot_offset(Context_class, "extra", Context_slots);
	assert_slots_len(Context_class, Context_slots);
	protect_slot(Context_class, "limits");
	protect_slot(Context_class, "screen");
	protect_slot(Context_class, "fbo");
	remove_init(Context_class);

	int Buffer_slots = 0;
	Buffer_class = detect_class(moderngl, "Buffer", Buffer_slots);
	Buffer_class_mglo = slot_offset(Buffer_class, "_Buffer__mglo", Buffer_slots);
	Buffer_class_size = slot_offset(Buffer_class, "size", Buffer_slots);
	Buffer_class_extra = slot_offset(Buffer_class, "extra", Buffer_slots);
	assert_slots_len(Buffer_class, Buffer_slots);
	protect_slot(Buffer_class, "size");
	remove_init(Buffer_class);

	initialized = true;
}

extern "C" PyObject * PyInit_mgl() {
	PyObject * module = PyModule_Create(&mgl_def);
	return module;
}