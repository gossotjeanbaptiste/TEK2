#include "../include/Parser.hpp"
#include "../include/Components/ComponentFactory.hpp"
#include <algorithm>


/**
 * @brief Analyse un fichier de description de circuit NanoTekSpice et construit un circuit.
 * 
 * Lit un fichier .nts et construit le circuit en traitant deux sections principales :
 * 1. Section .chipsets: - definit tous les composants du circuit
 * 2. Section .links: - etablit les connexions entre les broches des composants
 * 
 * L'analyseur est base sur les lignes et gere :
 * - Les commentaires (lignes commencant par '#')
 * - Les lignes vides (automatiquement ignores)
 * - Les en-tetes de section ('.chipsets:', '.links:')
 * - Les declarations de composants et les connexions de broches
 * 
 * @param filename Le chemin du fichier NanoTekSpice a analyser (extension .nts generalement)
 * @param circuit Une reference a l'objet Circuit a remplir avec les composants et liens analyses
 * 
 * @throws ParserException si le fichier ne peut pas etre ouvert
 * @throws ParserException si la definition du circuit est invalide (pas de chipsets)
 * @throws std::runtime_error si le type de composant est inconnu
 * 
 * @note Le circuit est construit de maniere incrementale lors de l'analyse du fichier.
 * @note Tous les espaces et tabulations sont normalises lors de l'analyse.
 * @note Les commentaires sont supprimes avant le traitement de chaque ligne.
// ! @note Dans le cadre actuel avec les cotinues dans les if (line == ".chipsets:") 
// ! si on avait mis l'appel de la fonction parseChipset au lieu de _currentSection = CHIPSETS
// ! on aurait traité que la ligne actuel et pas les lignes suivantes nécessaires
 */
void nts::Parser::parseFile(const std::string &filename, nts::Circuit &circuit) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw ParserException("Could not open file: " + filename);

    std::string line;
    while (std::getline(file, line)) {
        line = cleanLine(line);
        if (line.empty()) continue;

        if (line == ".chipsets:") {
            _currentSection = CHIPSETS;
            continue;
        } else if (line == ".links:") {
            _currentSection = LINKS;
            continue;
        }
        if (_currentSection == CHIPSETS)
            parseChipsets(line, circuit);
        else if (_currentSection == LINKS)
            parseLinks(line, circuit);
    }
    if (circuit.isEmpty())
        throw ParserException("No chipsets in the circuit.");
}

/**
 * @brief Nettoie une ligne brute du fichier d'entree pour le traitement.
 * 
 * Effectue les operations suivantes sur la ligne d'entree :
 * 1. Supprime les commentaires (tout apres '#')
 * 2. Remplace toutes les tabulations par des espaces pour une coherence des espaces
 * 3. Supprime les espaces de debut et de fin
 * 
 * Cette fonction est essentielle pour une analyse appropriee car elle :
 * - Elimine les commentaires qui seraient autrement analyses comme contenu
 * - Normalise les espaces a travers les differents formats d'entree
 * - Supprime les lignes vides en retournant une chaine vide
 * 
 * @param line La ligne brute du fichier d'entree
 * 
 * @return Une chaine nettoyee avec commentaires supprimes et espaces coupes.
 *         Retourne une chaine vide si la ligne est vide ou contient uniquement des espaces.
 * 
 * Exemple :
 * @code
 * Entree:  "  and gate  # Ceci est le composant AND  "
 * Sortie: "and gate"
 * @endcode
 */
std::string nts::Parser::cleanLine(std::string line) {
    size_t commentPos = line.find('#');
    if (commentPos != std::string::npos) // ! npos -> no position, retour de find('#') si y a pas de commentaire 
        line = line.substr(0, commentPos);
    std::replace(line.begin(), line.end(), '\t', ' ');

    size_t first = line.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = line.find_last_not_of(' ');
    return line.substr(first, (last - first + 1));
}


/**
 * @brief Analyse une ligne de declaration de chipset (composant) et l'ajoute au circuit.
 * 
 * Extrait le type et le nom du composant d'une ligne analysee et cree le composant
 * dans le circuit. Le format de la ligne doit etre : "<type> <name>"
 * 
 * @param line Une ligne nettoyee contenant la declaration du composant
 * @param circuit Une reference a l'objet Circuit auquel ajouter le composant
 * 
 * @throws ParserException si le format de la ligne est invalide (type ou nom manquant)
 * @throws std::runtime_error si le type de composant est inconnu
 * @throws std::runtime_error si un composant avec le meme nom existe deja
 */
void nts::Parser::parseChipsets(const std::string &line, nts::Circuit &circuit) {
    std::stringstream ss(line);
    std::string type, name;

    if (!(ss >> type >> name)) // ! si l'extraction a echoué
        throw ParserException("Invalid chipset declaration: " + line);
    circuit.addComponent(type, name);
}


/**
 * @brief Analyse une ligne de declaration de lien (connexion) et etablit la connexion.
 * 
 * Extrait les informations de connexion d'une ligne analysee et cree un lien
 * bidirectionnel entre deux broches de composants. Le format de la ligne doit etre : "<comp1>:<pin1> <comp2>:<pin2>"
 * 
 * Les deux composants et leurs broches specifices doivent exister pour que le lien soit cree.
 * La connexion est bidirectionnelle - les deux composants sont conscients de la connexion.
 * 
 * @param line Une ligne nettoyee contenant la declaration du lien
 * @param circuit Une reference a l'objet Circuit contenant les composants a lier
 * 
 * @throws ParserException si le format de la ligne est invalide (deux-points manquant dans la specification de broche)
 * @throws ParserException si le numero de broche ne peut pas etre analyse en entier
 * @throws std::runtime_error si l'un des composants n'existe pas
 * 
 * Exemples de lignes valides :
 * @code
 * in_1:1 gate:1
 * in_2:1 gate:2
 * gate:3 out:1
 * @endcode
 * 
 * Explication du format :
 * - "in_1:1" signifie : composant "in_1", broche 1
 * - "gate:1" signifie : composant "gate", broche 1
 * - La connexion va de la broche 1 de in_1 a la broche 1 de gate
 */
void nts::Parser::parseLinks(const std::string &line, nts::Circuit &circuit) {
    std::stringstream ss(line);
    std::string part1, part2;

    if (!(ss >> part1 >> part2))
        throw ParserException("Invalid link declaration: " + line);
    // ! methode specifique a parseLinks, le lambda de cette fonction evite de dupli le code 
    // ! lambda = fonction sans nom definit directement dans le code
    // ! [] = definit quelles var ext le lambda peut utiliser
    auto parsePin = [](const std::string &str) {
        size_t colonPos = str.find(':');
        if (colonPos == std::string::npos)
            throw ParserException("Invalid link format (missing ':'): " + str);
        std::string name = str.substr(0, colonPos);
        size_t pin = std::stoul(str.substr(colonPos + 1)); // * stoul -> convertie une string vers un unsigned int
        return std::make_pair(name, pin); // ! creer un couple clé valeur 
    };

    auto link1 = parsePin(part1);
    auto link2 = parsePin(part2);
    circuit.addLink(link1.first, link1.second, link2.first, link2.second);
}