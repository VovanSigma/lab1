#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <sstream>
#include <memory>

// === Базовий клас для ігрових сутностей ===
class GameEntity {
public:
    virtual std::string toString() const = 0;
    virtual ~GameEntity() = default;
};

// === Клас предметів спорядження ===
class Item : public GameEntity {
private:
    std::string name;
    int attackBoost;
    int defenseBoost;

public:
    Item(std::string name, int attack, int defense)
        : name(std::move(name)), attackBoost(attack), defenseBoost(defense) {}

    std::string toString() const override {
        std::ostringstream oss;
        oss << name << " [ATK: " << attackBoost << ", DEF: " << defenseBoost << "]";
        return oss.str();
    }

    int getAttackBoost() const { return attackBoost; }
    int getDefenseBoost() const { return defenseBoost; }
};

// === Базовий клас для персонажів ===
class Character : public GameEntity {
protected:
    std::string name;
    int health;
    int attack;
    int defense;
    int level;
    int experience;
    std::vector<Item> inventory;

public:
    Character(std::string name, int health, int attack, int defense)
        : name(std::move(name)), health(health), attack(attack), defense(defense), level(1), experience(0) {}

    std::string getName() const { return name; }
    int getDefense() const { return defense; }

    virtual void levelUp() {
        level++;
        health += 10;
        attack += 5;
        defense += 5;
    }

    virtual void addItem(const Item& item) {
        inventory.push_back(item);
        attack += item.getAttackBoost();
        defense += item.getDefenseBoost();
    }

    virtual int calculateDamage() const {
        return attack + (rand() % 5);
    }

    bool isAlive() const { return health > 0; }

    std::string toString() const override {
        std::ostringstream oss;
        oss << name << " [HP: " << health << ", ATK: " << attack
            << ", DEF: " << defense << ", LVL: " << level << "]";
        return oss.str();
    }

    virtual void takeDamage(int damage) {
        health -= damage;
        if (health < 0) health = 0;
    }

    virtual void gainExperience(int amount) {
        experience += amount;
        while (experience >= level * 100) {
            experience -= level * 100;
            levelUp();
        }
    }
};


// === Успадковані класи персонажів ===
class Warrior : public Character {
public:
    Warrior(std::string name)
        : Character(std::move(name), 100, 20, 10) {}

    void levelUp() override {
        Character::levelUp();
        attack += 5; // Воїн отримує більше атаки
    }
};

class Mage : public Character {
public:
    Mage(std::string name)
        : Character(std::move(name), 80, 25, 5) {}

    void levelUp() override {
        Character::levelUp();
        health += 5; // Маг отримує більше здоров'я
    }

    int calculateDamage() const override {
        return attack + (rand() % 10); // Маг має сильніший випадковий фактор
    }
};

// === Моделювання боїв ===
class Battle {
public:
    static void fight(Character& c1, Character& c2) {
        std::cout << "Battle starts between " << c1.toString() << " and " << c2.toString() << "\n";
        while (c1.isAlive() && c2.isAlive()) {
            int damageToC2 = std::max(0, c1.calculateDamage() - c2.getDefense());
            c2.takeDamage(damageToC2);
            std::cout << c1.getName() << " deals " << damageToC2 << " damage to " << c2.getName() << "\n";

            if (!c2.isAlive()) {
                std::cout << c2.getName() << " is defeated!\n";
                c1.gainExperience(50);
                return;
            }

            int damageToC1 = std::max(0, c2.calculateDamage() - c1.getDefense());
            c1.takeDamage(damageToC1);
            std::cout << c2.getName() << " deals " << damageToC1 << " damage to " << c1.getName() << "\n";

            if (!c1.isAlive()) {
                std::cout << c1.getName() << " is defeated!\n";
                c2.gainExperience(50);
            }
        }
    }
};


// === Генерація персонажів і предметів ===
template <typename T>
T generateRandomCharacter(const std::string& name) {
    if constexpr (std::is_same<T, Warrior>::value) {
        return Warrior(name);
    } else if constexpr (std::is_same<T, Mage>::value) {
        return Mage(name);
    } else {
        throw std::invalid_argument("Unsupported character type");
    }
}

Item generateRandomItem() {
    static const std::vector<std::string> itemNames = {"Sword", "Shield", "Amulet", "Ring"};
    std::string name = itemNames[rand() % itemNames.size()];
    int attackBoost = rand() % 10 + 1;
    int defenseBoost = rand() % 10 + 1;
    return Item(name, attackBoost, defenseBoost);
}

// === Сортування ===
template <typename T>
void insertionSort(std::vector<T>& vec) {
    for (size_t i = 1; i < vec.size(); ++i) {
        T key = vec[i];
        int j = i - 1;
        while (j >= 0 && vec[j] > key) {
            vec[j + 1] = vec[j];
            --j;
        }
        vec[j + 1] = key;
    }
}

// === Основна програма ===
int main() {
    srand(time(0));

    // Генерація персонажів
    Warrior warrior = generateRandomCharacter<Warrior>("Hero");
    Mage mage = generateRandomCharacter<Mage>("Enemy");

    // Додавання предметів
    warrior.addItem(generateRandomItem());
    mage.addItem(generateRandomItem());

    // Бій
    Battle::fight(warrior, mage);


    return 0;
}