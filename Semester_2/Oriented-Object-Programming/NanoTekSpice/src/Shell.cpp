#include "../include/Shell.hpp"
#include <iostream>
#include <sstream>
#include <csignal>
#include <algorithm>

static bool g_loop_interrupted = false;

/**
 * @brief Gestionnaire de signal pour l'interruption SIGINT
 * @param signal Le numero de signal recu par le processus
 * @return void
 * 
 * Cette fonction gere le signal SIGINT (Ctrl+C) en definissant le drapeau global
 * g_loop_interrupted a true, permettant a la boucle d'evenement principale de quitter
 * gracieusement lorsque l'utilisateur interrompt le programme.
 */
void signalHandler(int signal)
{
    if (signal == SIGINT)
        g_loop_interrupted = true;
}

Shell::Shell() : _running(true)
{
    // Default constructor
}

Shell::Shell(const Shell& other) : _running(other._running)
{
    // Copy constructor
}

/**
 * @brief Operateur d'affectation pour la classe Shell
 * 
 * Assigne l'etat d'un autre objet Shell a cet objet Shell.
 * Effectue une copie superficielle de l'etat d'execution pour eviter les problemes d'auto-affectation.
 * 
 * @param other L'objet Shell a copier
 * @return Une reference a cet objet Shell pour permettre le chainace des affectations
 * 
 * @note Utilise la protection du modele copy-and-swap avec verification d'auto-affectation
 */
Shell& Shell::operator=(const Shell& other)
{
    if (this != &other)
        _running = other._running;
    return *this;
}

/// @brief Definit le circuit pour le Shell
/// @param circuit Un pointeur unique vers un objet nts::Circuit a deplacer dans le Shell
/// @details Transfère la propriete de l'objet circuit a l'instance Shell en utilisant la semantique de deplacement
void Shell::setCircuit(std::unique_ptr<nts::Circuit>&& circuit)
{
    _circuit = std::move(circuit);
}

void Shell::display(std::ostream& os)
{
    (void)os;
}

/**
 * @brief Execute la commande d'affichage pour montrer le circuit actuel.
 * 
 * Affiche l'etat actuel du circuit charge. Si aucun circuit n'est charge,
 * un message d'erreur est imprime sur stderr et la fonction revient tot.
 * 
 * @return void
 * @throws Aucune
 * 
 * @note Necessite qu'un circuit soit charge via Shell::loadCircuit() ou similaire.
 *       Affichera un message d'erreur si _circuit est nullptr.
 * 
 * @see Circuit::display()
 */
void Shell::executeDisplay()
{
    if (!_circuit) {
        std::cerr << "Error: No circuit loaded" << std::endl;
        return;
    }
    _circuit->display();
}

/**
 * @brief Definit une valeur d'entree dans le circuit en fonction de la chaine d'entree fournie.
 * 
 * Analyse une chaine au format "input_name=value" et dfinit l'entree specifiee
 * a la valeur donnee. Le nom d'entree et la valeur sont coupes des espaces avant le traitement.
 * 
 * @param input Une chaine contenant l'affectation d'entree au format "input_name=value"
 * 
 * @details
 * - La chaine d'entree doit contenir un caractere '=' separant le nom d'entree de la valeur.
 * - Le nom d'entree et la valeur sont coupes des espaces de debut et de fin.
 * - La valeur doit etre l'une des suivantes : "0", "1", ou "U".
 * - Un circuit doit etre charge avant de definir les valeurs d'entree.
 * 
 * @throws Les exceptions de _circuit->setInputValue() sont interceptees et leurs messages
 *         sont imprimes sur stderr.
 * 
 * @return void
 * 
 * @note Les messages d'erreur sont imprimes sur stderr si :
 *       - Le format d'entree est invalide (caractere '=' manquant)
 *       - La valeur n'est pas l'une des options valides (0, 1, U)
 *       - Aucun circuit n'a ete charge
 *       - Une exception est levee par la methode setInputValue du circuit
 */
void Shell::executeSetInput(const std::string& input)
{
    size_t equalPos = input.find('=');

    if (equalPos == std::string::npos) {
        std::cerr << "Error: Invalid input format. Use 'input_name=value'" << std::endl;
        return;
    }
    
    std::string name = input.substr(0, equalPos);
    std::string value = input.substr(equalPos + 1);

    name.erase(name.find_last_not_of(" \t") + 1);
    name = name.substr(name.find_first_not_of(" \t"));
    value.erase(value.find_last_not_of(" \t") + 1);
    value = value.substr(value.find_first_not_of(" \t"));
    if (value != "0" && value != "1" && value != "U") {
        std::cerr << "Error: Value must be 0, 1, or U" << std::endl;
        return;
    }
    if (!_circuit) {
        std::cerr << "Error: No circuit loaded" << std::endl;
        return;
    }
    try {
        _circuit->setInputValue(name, value);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

/**
 * @brief Execute une simulation du circuit charge.
 * 
 * Cette methode execute la simulation sur le circuit actuellement charge.
 * Si aucun circuit n'a ete charge, un message d'erreur est imprime sur stderr
 * et la fonction revient sans effectuer aucune simulation.
 * 
 * @note Necessite qu'un circuit valide soit charge via Shell::loadCircuit() ou similaire.
 * @see _circuit
 * 
 * @return void
 */
void Shell::executeSimulate()
{
    if (!_circuit) {
        std::cerr << "Error: No circuit loaded" << std::endl;
        return;
    }
    _circuit->simulate();
}

/**
 * @brief Execute la boucle de simulation principale du circuit.
 * 
 * Cette fonction simule et affiche continuellement l'etat du circuit jusqu'a
 * etre interrompue. Elle configure un gestionnaire de signal pour gerer gracieusement
 * les interruptions SIGINT (Ctrl+C) et restaure le comportement de signal par defaut a la fin.
 * 
 * @throws Enregistre les erreurs sur stderr si aucun circuit n'est charge ou si une exception
 *         se produit pendant les operations de simulation/affichage.
 * 
 * @note Modifie la variable d'etat globale g_loop_interrupted pour controler
 *       l'execution de la boucle. Le gestionnaire de signal doit etre defini et accessible.
 * 
 * @return void
 * 
 * @pre _circuit doit etre initialise et valide avant d'appeler cette fonction.
 * 
 * @post Les gestionnaires de signal sont restaures a l'etat par defaut apres l'execution.
 */
void Shell::executeLoop()
{
    if (!_circuit) {
        std::cerr << "Error: No circuit loaded" << std::endl;
        return;
    }
    std::signal(SIGINT, signalHandler);
    g_loop_interrupted = false;
    while (!g_loop_interrupted) {
        try {
            _circuit->simulate();
            _circuit->display();
        } catch (const std::exception& e) {
            std::cerr << "Error during simulation: " << e.what() << std::endl;
            break;
        }
    }
    std::signal(SIGINT, SIG_DFL);
    g_loop_interrupted = false;
}

/**
 * @brief Traite et execute une commande saisie par l'utilisateur.
 * 
 * Cette fonction traite une commande d'entree utilisateur en commencant par couper les
 * espaces de debut et de fin. Elle divise ensuite la commande en arguments individuels
 * separes par des espaces, sauf pour les arguments contenant '=' qui peuvent contenir des espaces.
 * Chaque argument est traite independamment :
 * - "exit" : Arrete le shell en definissant _running a false
 * - "display" : Execute l'operation d'affichage
 * - "simulate" : Execute une etape de simulation
 * - "loop" : Execute l'operation de boucle
 * - Arguments contenant '=' : Traite comme affectation d'entree et appelle executeSetInput
 * - Tout autre argument : Affiche un message d'erreur sur stderr
 * 
 * @param command La chaine de commande saisie par l'utilisateur, pouvant contenir
 *                plusieurs arguments separes par des espaces
 * 
 * @return void
 * 
 * @note Les commandes vides (apres coupure d'espaces) sont silencieusement ignorees
 * @note Les commandes inconnues produisent un message d'erreur sur l'erreur standard
 * @note Permet de passer plusieurs arguments en une seule ligne : 
 *       "input1=1 input2=0 simulate display"
 */
void Shell::handleCommand(const std::string& command)
{
    std::string trimmed = command;
    std::istringstream iss(trimmed); // ! iss -> input string stream
    std::string arg;

    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    trimmed = trimmed.substr(trimmed.find_first_not_of(" \t\n\r"));
    if (trimmed.empty())
        return;
    
    iss.str(trimmed);
    while (iss >> arg) {
        if (arg == "exit")
            _running = false;
        else if (arg == "display")
            executeDisplay();
        else if (arg == "simulate")
            executeSimulate();
        else if (arg == "loop")
            executeLoop();
        else if (arg.find('=') != std::string::npos)
            executeSetInput(arg);
        else
            std::cerr << "Error: Unknown command '" << arg << "'" << std::endl;
    }
}

/**
 * @brief Démarre la boucle interactive du shell.
 * 
 * Lit continuellement l'entrée utilisateur depuis l'entrée standard et traite les commandes
 * jusqu'à ce que la fin du fichier soit atteinte ou que le shell soit arrêté (via le drapeau _running).
 * Affiche une invite ("> ") avant chaque entrée et vide le tampon de sortie
 * pour assurer que l'invite est visible immédiatement.
 * 
 * @return void
 * 
 * @note La boucle se termine quand :
 *       - std::getline échoue (EOF ou erreur d'entrée)
 *       - Le drapeau _running est défini à false par handleCommand ou d'autres moyens
 */
void Shell::run()
{
    std::string line;

    while (true) {
        std::cout << "> ";
        std::cout.flush();
        if (!std::getline(std::cin, line))
            break;
        handleCommand(line);
        if (!_running)
            break;
    }
}
