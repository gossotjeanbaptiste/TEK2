pub fn compute_survivable_days(water_quantity: f32, water_cycles: u32, food_quantity: f32, nb_crew: u32) -> u32
{
    let total_water = water_quantity * water_cycles as f32;
    let water_consumption_per_day = 2.0 * nb_crew as f32;
    let days_with_water = (total_water / water_consumption_per_day) as u32;

    let food_in_grams = food_quantity * 1000.0;
    let food_consumption_per_day = 500.0 * nb_crew as f32;
    let days_with_food = (food_in_grams / food_consumption_per_day) as u32;

    println!("If the mission can embark {}L of water, {}kg of food and the water recycler can do {} water cycles... With {} crew members, there is enough water for {} day(s) and enough food for {} day(s).", water_quantity, food_quantity, water_cycles, nb_crew, days_with_water, days_with_food );

    if days_with_water < days_with_food {
        days_with_water
    } else {
        days_with_food
    }
}
