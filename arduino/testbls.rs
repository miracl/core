// Example program for Arduino nano33iot only
// Some code borrowed from https://github.com/atsamd-rs/atsamd
//
// Put this code into examples folder for nano33iot board
// Uncomment #![no_std] in lib.rs
// Change line in config32.py to os.system("cargo rustc --target thumbv6m-none-eabi --manifest-path mcore"+slashtext+"Cargo.toml --release --lib")
// Edit bls.rs to NOT use precomputation (uses too much memory for embedded)
//
// Build Rust library using modified config32.py and selecting bn254 curve
// Requires terminal emulator program running on host for I/O - e.g. termite
// cargo build --release --example testbls --features="usb"
// arm-none-eabi-objcopy -O binary target/thumbv6m-none-eabi/release/examples/testbls target/testbls.bin
// bossac -i -d -U true -i -e -w -v target/testbls.bin -R 
//

#![no_std]
#![no_main]

extern crate arduino_nano33iot as hal;
extern crate cortex_m;
extern crate panic_halt;
extern crate usb_device;
extern crate usbd_serial;

use hal::clock::GenericClockController;
use hal::prelude::*;
use hal::entry;
use hal::pac::{interrupt, CorePeripherals, Peripherals, TC3};
use hal::usb::UsbBus;
use usb_device::bus::UsbBusAllocator;
use usb_device::prelude::*;
use usbd_serial::{SerialPort, USB_CLASS_CDC};
//use cortex_m::asm::delay as cycle_delay;
use cortex_m::peripheral::NVIC;
use numtoa::NumToA;
use core::sync::atomic;

static INTERRUPT_FIRED: atomic::AtomicBool = atomic::AtomicBool::new(false);
static mut USB_ALLOCATOR: Option<UsbBusAllocator<UsbBus>> = None;
static mut USB_BUS: Option<UsbDevice<UsbBus>> = None;
static mut USB_SERIAL: Option<SerialPort<UsbBus>> = None;
static mut TIMER:usize = 0;

// Deal with interrupts
#[interrupt]
fn USB() {
    unsafe {
        USB_BUS.as_mut().map(|usb_dev| {
            USB_SERIAL.as_mut().map(|serial| {
                usb_dev.poll(&mut [serial]);
                // Keep the other side happy
                let mut buf = [0u8; 16];
                let _ = serial.read(&mut buf);
            });
        }); 
    }
}

#[interrupt]
fn TC3() {
    INTERRUPT_FIRED.store(true, atomic::Ordering::Relaxed);
    unsafe {
        TIMER+=1;
        TC3::ptr()
            .as_ref()
            .unwrap()
            .count16()
            .intflag
            .modify(|_, w| w.ovf().set_bit());
    }
}


// USB IO Routines
// Print out an Ascii string
fn printstring(s: &str) {
 // Turn off interrupts so we don't fight with the interrupt
    cortex_m::interrupt::free(|_| unsafe {
        USB_BUS.as_mut().map(|_| {
            USB_SERIAL.as_mut().map(|serial| {
                // Skip errors so we can continue the program
                let _ = serial.write(s.as_bytes());
            });
        })
    });
}

// Print a number with optional padding with leading zeros, to given base
// eg printnumber(10,16,2) -> prints 0A
fn printnumber(n: usize,base: usize,pad: usize) {
    let mut buf:[u8;20]=[0;20]; // some workspace for numtoa
    let s=n.numtoa(base,&mut buf);    
    let mut buff:[u8;20]=[0;20];
    let mut len=s.len();
    if pad==0 {
        for i in 0..len {
            buff[i]=s[i];
         }
    } else {
        if len>=pad {
            for i in 0..pad {
                buff[i]=s[i];
            }
        } else {
            for i in 0..pad-len {
                buff[i]=48; // leading '0'
            }
            for i in pad-len..pad {
                buff[i]=s[i+len-pad];
            }
        }
        len=pad;
    }

// Turn off interrupts so we don't fight with the interrupt
    cortex_m::interrupt::free(|_| unsafe {
        USB_BUS.as_mut().map(|_| {
            USB_SERIAL.as_mut().map(|serial| {
                // Skip errors so we can continue the program
                let _ = serial.write(&buff[0..len]);
            });
        })
    });
}

extern crate mcore;
use mcore::rand::{RAND, RAND_impl};

fn printbinary(array: &[u8]) {
    for i in 0..array.len() {
        printnumber(array[i] as usize,16,2)
    }
    printstring("\n");
}

fn restart_clock() {
    unsafe {
        TIMER=0;
    }
}

fn printtime() {
    let t:usize;
    unsafe{
        t=TIMER;
        TIMER=0;
    }    
    printstring("duration= ");
    let secs=t/10;
    let tens=t%10;
    printnumber(secs,10,0);
    printstring(".");
    printnumber(tens,10,1);
    printstring("\n");
}

fn bls_bn254(rng: &mut impl RAND) {
    use mcore::bn254::bls;

    const BFS: usize = bls::BFS;
    const BGS: usize = bls::BGS;

    let mut s: [u8; BGS] = [0; BGS];

    const G1S: usize =  BFS + 1; // Group 1 Size - compressed 
    const G2S: usize = 2 * BFS + 1; // Group 2 Size - compressed

    let mut w: [u8; G2S] = [0; G2S];
    let mut sig: [u8; G1S] = [0; G1S];
    let mut ikm: [u8; 32] = [0; 32];

    for i in 0..32 {
        //ikm[i] = (i+1) as u8;
        ikm[i]=rng.getbyte();
    }

    let m = "This is a test message";

    let mut res = bls::init();
    if res != 0 {
        printstring("Failed to Initialize\n");
        return;
    }

    restart_clock();
    res = bls::key_pair_generate(&ikm, &mut s, &mut w);
    if res != 0 {
        printstring("Failed to Generate Keys\n");
        return;
    }
    printtime();

    printstring("Private key : 0x");
    printbinary(&s);
    printstring("Public  key : 0x");
    printbinary(&w);

    restart_clock();
    bls::core_sign(&mut sig, &m.as_bytes(), &s);
    printstring("Signature : 0x");
    printbinary(&sig);
    printtime();

    restart_clock();
    res = bls::core_verify(&sig, &m.as_bytes(), &w);
    if res == 0 {
        printstring("Signature is OK\n");
    } else {
        printstring("Signature is *NOT* OK\n");
    }
    printtime();
}

#[entry]
fn main() -> ! {
// prepare hardware
    let mut peripherals = Peripherals::take().unwrap();
    let mut core = CorePeripherals::take().unwrap();
    let mut clocks = GenericClockController::with_internal_32kosc(
        peripherals.GCLK,
        &mut peripherals.PM,
        &mut peripherals.SYSCTRL,
        &mut peripherals.NVMCTRL,
    );
    let gclk0 = clocks.gclk0();
    let mut tc3 = hal::timer::TimerCounter::tc3_(
        &clocks.tcc2_tc3(&gclk0).unwrap(),
        peripherals.TC3,
        &mut peripherals.PM,
    );

// create timer = ticks 10 times per second
    tc3.start(10.hz());
    tc3.enable_interrupt();

    let pins = hal::Pins::new(peripherals.PORT);
    //let mut red_led = pins.led_sck.into_open_drain_output(&mut pins.port);

// set up USB port
    let bus_allocator = unsafe {
        USB_ALLOCATOR = Some(hal::usb_allocator(
            peripherals.USB,
            &mut clocks,
            &mut peripherals.PM,
            pins.usb_dm,
            pins.usb_dp,
        ));
        USB_ALLOCATOR.as_ref().unwrap()
    };

    unsafe {
        USB_SERIAL = Some(SerialPort::new(&bus_allocator));
        USB_BUS = Some(
            UsbDeviceBuilder::new(&bus_allocator, UsbVidPid(0x2222, 0x3333))
                .manufacturer("Fake company")
                .product("Serial port")
                .serial_number("TEST")
                .device_class(USB_CLASS_CDC)
                .build(),
        );
    }

// Enable Interrupts for USB and for a timer
    unsafe {
        core.NVIC.set_priority(interrupt::USB, 2);
        NVIC::unmask(interrupt::USB);
        core.NVIC.set_priority(interrupt::TC3, 1);
        NVIC::unmask(interrupt::TC3);
    }


// ****************************************************************************************
// Application starts here
/* Some hardware test code
    let mut count:usize=0;
    // Flash the LED in a spin loop to demonstrate that USB is
    // entirely interrupt driven.
    loop {
        count+=1;
        cycle_delay(5 * 1024 * 1024);
        red_led.toggle();

        printstring("Hello World\n");
        printnumber(89,16,2);
        printnumber(56,16,2);
        printnumber(7,16,2);
        printstring("\n");
        printstring("TIMER= ");
        unsafe{
            printnumber(TIMER,10,0);
        }
        printstring("\n");
        printstring("loop count= ");
        printnumber(count,10,0);
        printstring("\n");
    } */

    let mut raw: [u8; 100] = [0; 100];

    let mut rng = RAND_impl::new();
    rng.clean();
    for i in 0..100 {
        raw[i] = i as u8
    }

    rng.seed(100, &raw);
    loop {
        printstring("Testing BLS signature for curve bn254\n");
        bls_bn254(&mut rng);
    }
}
