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
            println!("Button press {}!", be.button);
            if be.button == 1 {
                maybe_mut_ref(tool)
                     .map(|tool| {
                        unsafe { tool_draw(tool, be.x, be.y) };
                        tool.holding = 1
                     });
                    
            }
            maybe_ref(tool).map(|t| println!("holding: {}", t.holding));
            CBool::True
        }, Some(XEvent::ButtonRelease(be)) => {
            println!("Button release!");
            if be.button == 1 {
                maybe_mut_ref(tool)
                     .map(|tool| tool.holding = 0);
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
