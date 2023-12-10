mod x11;
use crate::x11::*;

#[repr(C)]
pub struct Tool {
    holding: i32
}

extern "C" {
    fn open_display();
    fn init_canvas();
    fn del_canvas();
    fn close_display();
    fn main_loop();
    fn tool_draw(tool: &mut Tool, x: i32, y: i32);
}

#[no_mangle]
pub extern "C" fn accept_event(tool: *mut Tool, ptr: *const Void) -> CBool {
    match XEvent::from_ptr(ptr) {
        Some(XEvent::ButtonPress(be)) => {
            if be.button == 1 {
                maybe_mut_ref(tool)
                     .map(|tool| {
                        unsafe { tool_draw(tool, be.x, be.y) };
                        tool.holding = 1
                     });
                    
            }
            CBool::True
        }, Some(XEvent::ButtonRelease(be)) => {
            if be.button == 1 {
                maybe_mut_ref(tool)
                     .map(|tool| tool.holding = 0);
            }
            CBool::True
        }, Some(XEvent::Motion(me)) => {
            if let Some(state) = maybe_mut_ref(tool) {
                unsafe { tool_draw(state, me.x, me.y) };
            }
            CBool::True
        }, _ => CBool::False
    }

}

fn main() {
    unsafe {
        open_display();
        init_canvas();
	
        main_loop();
	
        del_canvas();
        close_display();
    }
}