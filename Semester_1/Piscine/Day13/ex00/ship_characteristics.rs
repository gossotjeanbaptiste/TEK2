const DISCOVERIES: [&str; 4] = [
    "The Union discovered that the hottest planet in our solar system is Venus (about 450°C).",
    "The Union discovered that Mercury and Venus are the only planets in our solar system with no moons.",
    "The Union discovered that one day on Venus is longer than a year on Earth.",
    "The Union discovered that on Venus it snow metal and rains sulfuric acid.",
];

pub struct RocketBooster {
    height: f32,
    diameter: f32,
    last_check_time: u32,
}

pub struct FuelTank {
    height: f32,
    quantity: f32,
    max_quantity: f32,
    last_check_time: u32,
}

pub struct CrewCabin {
    nb_seats: u8,
    diameter: f32,
    last_check_time: u32,
}

pub struct SciencePayload {
    quantity: f32,
    max_quantity: f32,
    science_percentage: f32,
}

pub trait RocketComponent {
    fn new() -> Self;
    fn info(&self) -> String;
}

pub trait SafetyCheck {
    fn check_component(&mut self, current_time: u32) -> bool;
    fn is_safe(&self, current_time: u32) -> bool;
}

pub trait LoadContent {
    fn load(&mut self, to_load: f32);
    fn is_full(&self) -> bool;
}

impl RocketComponent for RocketBooster {
    fn new() -> Self {
        println!("Rocket Booster created with a height of 32.3 meters.");
        RocketBooster {
            height: 32.3,
            diameter: 3.0,
            last_check_time: 0,
        }
    }

    fn info(&self) -> String {
        format!(
            "This Rocket Booster is 32.3 meters tall and {} meters wide. It was last checked on day {} of its life.",
            self.diameter as u32, self.last_check_time
        )
    }
}

impl SafetyCheck for RocketBooster {
    fn check_component(&mut self, current_time: u32) -> bool {
        if current_time < self.last_check_time {
            return false;
        }
        self.last_check_time = current_time;
        true
    }

    fn is_safe(&self, current_time: u32) -> bool {
        if self.last_check_time == 0 || current_time < self.last_check_time {
            return false;
        }
        current_time - self.last_check_time <= 12
    }
}

impl RocketComponent for FuelTank {
    fn new() -> Self {
        println!("Fuel Tank created with a max quantity of 220 tons.");
        FuelTank {
            height: 30.0,
            quantity: 0.0,
            max_quantity: 220.0,
            last_check_time: 0,
        }
    }

    fn info(&self) -> String {
        let quantity_str = if self.quantity.fract() == 0.0 {
            format!("{}", self.quantity as u32)
        } else {
            format!("{:.2}", self.quantity)
        };
        format!(
            "This Fuel Tank is 30 meters tall and can contain up to 220 tons of propergol, currently {} tons inside. It was last checked on day {} of its life.",
            quantity_str, self.last_check_time
        )
    }
}

impl SafetyCheck for FuelTank {
    fn check_component(&mut self, current_time: u32) -> bool {
        if current_time < self.last_check_time {
            return false;
        }
        self.last_check_time = current_time;
        true
    }

    fn is_safe(&self, current_time: u32) -> bool {
        if self.last_check_time == 0 || current_time < self.last_check_time {
            return false;
        }
        current_time - self.last_check_time <= 6
    }
}

impl LoadContent for FuelTank {
    fn load(&mut self, to_load: f32) {
        self.quantity = (self.quantity + to_load).min(self.max_quantity);
    }

    fn is_full(&self) -> bool {
        self.quantity >= self.max_quantity
    }
}

impl RocketComponent for CrewCabin {
    fn new() -> Self {
        println!("Cabin of 4 person created.");
        CrewCabin {
            nb_seats: 4,
            diameter: 2.5,
            last_check_time: 0,
        }
    }

    fn info(&self) -> String {
        format!(
            "This cabin has {} seats arranged in a circle {:.2} meters large. It was last checked on day {} of its life.",
            self.nb_seats, self.diameter, self.last_check_time
        )
    }
}

impl SafetyCheck for CrewCabin {
    fn check_component(&mut self, current_time: u32) -> bool {
        if current_time < self.last_check_time {
            return false;
        }
        self.last_check_time = current_time;
        true
    }

    fn is_safe(&self, current_time: u32) -> bool {
        if self.last_check_time == 0 || current_time < self.last_check_time {
            return false;
        }
        current_time - self.last_check_time <= 16
    }
}

impl RocketComponent for SciencePayload {
    fn new() -> Self {
        println!("Science Payload created with a max quantity of 1142 kilos.");
        SciencePayload {
            quantity: 0.0,
            max_quantity: 1142.0,
            science_percentage: 0.0,
        }
    }

    fn info(&self) -> String {
        let max_str = if self.quantity == 0.00 {
            format!("{:.2}", self.max_quantity as u32)
        } else {
            format!("{:.2}", self.max_quantity)
        };
        let quantity_str = if self.quantity.fract() == 0.0 && self.quantity == 0.0 {
            format!("{}", self.quantity as u32)
        } else {
            format!("{:.2}", self.quantity)
        };
        let percentage_str = if self.science_percentage.fract() == 0.00 && self.science_percentage == 0.00 {
            format!("{:.2}%", self.science_percentage as u32)
        } else {
            format!("{:.2}%", self.science_percentage)
        };
        format!(
            "This Science Payload can carry {} kilos of science material. Currently it contains {} kilos which means it has a science percentage of {}.",
            max_str, quantity_str, percentage_str
        )
    }
}

impl LoadContent for SciencePayload {
    fn load(&mut self, to_load: f32) {
        self.quantity = (self.quantity + to_load).min(self.max_quantity);
        self.science_percentage = (self.quantity / self.max_quantity) * 200.0;
    }

    fn is_full(&self) -> bool {
        self.quantity >= self.max_quantity
    }
}

impl SciencePayload {
    pub fn science(&self) -> f32 {
        let discovery_index = if self.science_percentage < 50.0 {
            0
        } else if self.science_percentage < 100.0 {
            1
        } else if self.science_percentage < 150.0 {
            2
        } else {
            3
        };
        println!("{}", DISCOVERIES[discovery_index]);
        self.science_percentage
    }
}