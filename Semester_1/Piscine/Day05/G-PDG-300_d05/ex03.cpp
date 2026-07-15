int main(void)
{
    unsigned int nature = 777;
    Mondas::Human human1("Korving", 164);
    Mondas::Human human2("Voga", 89);
    Mondas::Human human3("Lumic", 201);
    Mondas::Cyberfoundry cyberfoundry(nature);

    auto cyberman = cyberfoundry.buildCyberman(human1);
    auto cyberleader = cyberfoundry.buildCyberleader(human3);

    cyberfoundry.buildCyberman(human2);
    cyberfoundry.buildCyberleader(human2);
    cyberfoundry.recycle(cyberman);
    cyberfoundry.recycle(cyberleader);
    cyberfoundry.recycle(cyberleader);
    cyberman = cyberfoundry.buildCyberman(human2);
    cyberfoundry.recycle(cyberman);

    return 0;
}
