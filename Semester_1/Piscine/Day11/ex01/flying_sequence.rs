pub fn flying_sequence(mut countdown: u32, ignition: u32, secondary_activation: u32, thrust_duration: u32)
{
    if ignition > countdown || secondary_activation < 30 {
        println!("Wrong flying sequence, go back to the lab !");
        return;
    }

    let mut time = 0u32;
    let liftoff_time = countdown;
    let main_engine_cutoff = secondary_activation - 10;
    let main_engine_decoupling = secondary_activation - 9;
    let secondary_cutoff = secondary_activation + thrust_duration;

    while time < liftoff_time {
        if time == (liftoff_time - ignition) {
            println!("Main engine ignition");
        }
        if time < liftoff_time {
            println!("Liftoff in {}...", liftoff_time - time)
        }
        time += 1;
    }
    println!("Liftoff ! We have liftoff !");

    time = 0;
    while time <= secondary_cutoff {
        if time == main_engine_cutoff {
            if time % 10 == 0 {
                println!("T+{} : Everything is fine", time);
            }
            println!("T+{} : Main engine cutoff", time);
        } else if time == main_engine_decoupling {
            println!("T+{} : Main engine decoupling", time);
            if time % 10 == 0 {
                println!("T+{} : Everything is fine", time);
            }
        } else if time == secondary_activation {
            println!("T+{} : Secondary engines ignition", time);
            if time % 10 == 0 {
                println!("T+{} : Everything is fine", time);
            }
        } else if time == secondary_cutoff {
            println!("T+{} : Secondary engines cutoff. We're in orbit !", time);
        } else if time > 0 && time % 10 == 0 {
            println!("T+{} : Everything is fine", time);
        }
        time += 1;
    }
}

fn main() {
    flying_sequence(5, 6, 30, 35);
    println!("");
    flying_sequence(5, 2, 30, 35);
    println!("");
    flying_sequence(10, 3, 159, 35);
}