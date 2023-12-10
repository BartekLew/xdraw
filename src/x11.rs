
type Display = u64;
type Window = u64;
pub struct Void{}

#[repr(u64)]
pub enum CBool {
    True = 1,
    False = 0
}

pub fn maybe_ref<'a, T>(ptr: *const T) -> Option<&'a T> {
    unsafe {
        if ptr != 0 as *const T {
            Some(&*ptr)
        } else {
            None
        }
    }
}

pub fn maybe_mut_ref<'a, T>(ptr: *mut T) -> Option<&'a mut T> {
    unsafe {
        if ptr != 0 as *mut T {
            Some(&mut *ptr)
        } else {
            None
        }
    }
}

#[repr(C)]
pub struct XButtonEvent {
	evtype: i64, serial: u64, 
	send_event: CBool, diplay: Display,
    window: Window, root: Window, subwindow: Window,
    time: u64, pub x: i32, pub y: i32, x_root: i32, y_root: i32, // 72
    state: u32, pub button: u32, same_screen: CBool
}

#[repr(C)]
pub struct XMotionEvent {
	evtype: i64, serial: u64, 
	send_event: CBool, diplay: Display,
    window: Window, root: Window, subwindow: Window,
    time: u64, pub x: i32, pub y: i32, x_root: i32, y_root: i32, // 72
    state: u32, pub button: u32, is_hint: u8, same_screen: CBool
}

#[repr(C)]
pub enum XEvent<'a> {
    ButtonPress(&'a XButtonEvent),
    ButtonRelease(&'a XButtonEvent),
    Motion(&'a XMotionEvent),
    _Raw(&'a [u32;48])
}

impl<'a> XEvent<'a> {
    pub fn from_ptr(ptr: *const Void) -> Option<Self> {
        unsafe {
            maybe_ref::<i32>(ptr as *const i32)
                .and_then(|rf| match *rf {
                    4 => Some(Self::ButtonPress(&*(ptr as *const XButtonEvent))),
                    5 => Some(Self::ButtonRelease(&*(ptr as *const XButtonEvent))),
                    6 => Some(Self::Motion(&*(ptr as *const XMotionEvent))),
                    _ => None
                })
        }
    }
}

