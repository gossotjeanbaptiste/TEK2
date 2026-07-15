use std::fmt::{self, Display};

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum MissionState {
    Planned,
    InProgress,
    Completed,
    Failed,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Rover {
    pub name: String,
    pub fuel_level: u8,
    pub map_sectors: Vec<u32>,
}

pub struct Mission {
    pub mission_name: String,
    pub rover: Rover,
    pub state: MissionState,
}

impl fmt::Debug for Mission {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("Mission")
            .field("mission_name", &self.mission_name)
            .field("rover", &self.rover)
            .field("state", &self.state)
            .finish()
    }
}

impl Display for MissionState {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            MissionState::Planned => write!(f, "PLANNED"),
            MissionState::InProgress => write!(f, "IN PROGRESS"),
            MissionState::Completed => write!(f, "COMPLETED"),
            MissionState::Failed => write!(f, "FAILED"),
        }
    }
}

impl Display for Rover {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{} (Fuel: {}%)", self.name, self.fuel_level)
    }
}

impl Display for Mission {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "[{}] {}\n> Assigned Rover: {}\n> Map Coverage: {} sectors",
            self.state,
            self.mission_name,
            self.rover,
            self.rover.map_sectors.len()
        )
    }
}

impl PartialEq for Mission {
    fn eq(&self, other: &Self) -> bool {
        self.mission_name == other.mission_name
    }
}

impl Eq for Mission {}

impl Ord for MissionState {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        use std::cmp::Ordering;
        match (self, other) {
            (a, b) if a == b => Ordering::Equal,
            (MissionState::Failed, _) => Ordering::Greater,
            (_, MissionState::Failed) => Ordering::Less,
            (MissionState::InProgress, _) => Ordering::Greater,
            (_, MissionState::InProgress) => Ordering::Less,
            (MissionState::Planned, MissionState::Completed) => Ordering::Greater,
            (MissionState::Completed, MissionState::Planned) => Ordering::Less,
            _ => Ordering::Equal,
        }
    }
}

impl PartialOrd for MissionState {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}