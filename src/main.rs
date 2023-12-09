extern "C" {
    fn open_display();
    fn init_canvas();
    fn del_canvas();
    fn close_display();
    fn main_loop();
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
