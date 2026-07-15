int main(void)
{
    Mondas::Cyberman::Inhibitor inhibitor;
    Mondas::Human human("Lumic", 201);
    Mondas::Cyberleader cyberleader(human);

    Mondas::Cyberman::Inhibitor testInhibitor;
    Mondas::Human testHuman("Voga");
    Mondas::Cyberman testCyberman(testHuman);

    cyberleader.upgrade(testCyberman);
    cyberleader.upgrade(testHuman);
    cyberleader.upgrade(testInhibitor);
    human.setInhibitor(&inhibitor);
    cyberleader.upgrade(testCyberman);
    inhibitor.set(true);

    std::cout << "---" << std::endl;

    cyberleader.upgrade(testHuman);
    cyberleader.upgrade(testCyberman);
    cyberleader.upgrade(testInhibitor);

    std::cout << "---" << std::endl;

    testCyberman.getHuman().setInhibitor(&testInhibitor);
    cyberleader.upgrade(testHuman);
    cyberleader.upgrade(testCyberman);
    cyberleader.upgrade(testInhibitor);

    return 0;
}
