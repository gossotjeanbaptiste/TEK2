int main(void)
{
    Mondas::Cyberman::Inhibitor inhibitor;
    Mondas::Human human1("Korving", 164, &inhibitor);
    Mondas::Human human2("Voga");

    {
        Mondas::Human human3("Lumic", &inhibitor);
    }

    const Mondas::Cyberman unit1(human1);
    unit1.think();
    inhibitor.set(true);
    std::cout << unit1.getIq() << std::endl;
    unit1.think();

    Mondas::Cyberman unit2(human2);
    unit2.think();
    unit2.getHuman().setInhibitor(&inhibitor);
    unit2.think();

    return 0;
}
