#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <sstream>
#include <memory>
#include <list> 

class GameEntity {
public:
    virtual std::string toString() const = 0;
    virtual ~GameEntity() = default;
};

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

    bool operator<(const Item& other) const {
        return (attackBoost + defenseBoost) < (other.attackBoost + other.defenseBoost);
    }

    bool operator>(const Item& other) const {
        return (attackBoost + defenseBoost) > (other.attackBoost + other.defenseBoost);
    }

    bool operator==(const Item& other) const {
        return (attackBoost + defenseBoost) == (other.attackBoost + other.defenseBoost);
    }
};

class Character : public GameEntity {
protected:
    std::string name;
    int health;
    int attack;
    int defense;
    int level;
    int experience;
    std::list<Item> inventory; 

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

    const std::list<Item>& getInventory() const {
        return inventory;
    }
};

class Warrior : public Character {
public:
    Warrior(std::string name)
        : Character(std::move(name), 100, 20, 10) {}

    void levelUp() override {
        Character::levelUp();
        attack += 5;      // Воїн отримує більше атаки
    } 
};

class Mage : public Character {
public:
    Mage(std::string name)
        : Character(std::move(name), 80, 25, 5) {}

    void levelUp() override {
        Character::levelUp();
        health += 5;     // Маг отримує більше здоров'я
    }

    int calculateDamage() const override {
        return attack + (rand() % 10);     // Маг має сильніший випадковий фактор
    }
};

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

template <typename T>
void insertionSort(std::list<T>& lst) {
    for (auto it = std::next(lst.begin()); it != lst.end(); ++it) {
        auto current = *it;
        auto prev = std::prev(it);
        while (prev != lst.begin() && *prev > current) {
            *it = *prev;
            --it;
            prev = std::prev(it);
        }
        *it = current;
    }
}

template <typename T>
void merge(std::list<T>& lst, std::list<T>& left, std::list<T>& right) {
    auto itL = left.begin(), itR = right.begin();
    while (itL != left.end() && itR != right.end()) {
        if (*itL < *itR) {
            lst.push_back(*itL);
            ++itL;
        } else {
            lst.push_back(*itR);
            ++itR;
        }
    }
    lst.insert(lst.end(), itL, left.end());
    lst.insert(lst.end(), itR, right.end());
}

template <typename T>
void mergeSort(std::list<T>& lst) {
    if (lst.size() <= 1) return;

    auto mid = std::next(lst.begin(), lst.size() / 2);
    std::list<T> left(lst.begin(), mid);
    std::list<T> right(mid, lst.end());

    mergeSort(left);
    mergeSort(right);
    lst.clear();
    merge(lst, left, right);
}

template <typename T>
void quickSort(std::list<T>& lst) {
    if (lst.size() <= 1) return;

    auto pivot = lst.front();
    std::list<T> left, right;

    for (auto it = std::next(lst.begin()); it != lst.end(); ++it) {
        if (*it < pivot) left.push_back(*it);
        else right.push_back(*it);
     }

    quickSort(left);
    quickSort(right);
    lst.clear();
    lst.splice(lst.end(), left);
    lst.push_back(pivot);
    lst.splice(lst.end(), right);
 }

Item generateRandomItem() {
    static const std::vector<std::string> itemNames = {"Sword", "Shield", "Amulet", "Ring"};
    std::string name = itemNames[rand() % itemNames.size()];
    int attackBoost = rand() % 10 + 1;
    int defenseBoost = rand() % 10 + 1;
    return Item(name, attackBoost, defenseBoost);
}

int main() {
    srand(time(0));

    Warrior warrior("Hero");
    Mage mage("Enemy");

    std::list<Item> items;
    for (int i = 0; i < 5; ++i) {
        items.push_back(generateRandomItem());
    }

    std::cout << "Items before sorting:\n";
    for (const auto& item : items) {
        std::cout << "  - " << item.toString() << "\n";
     }

    insertionSort(items);  

    std::cout << "\nItems after sorting (Insertion Sort):\n";
    for (const auto& item : items) {
        std::cout << "  - " << item.toString() << "\n";
    }

    // Додаємо найкращий предмет (перший після сортування) персонажам
    if (!items.empty()) {
        warrior.addItem(items.front());  
        mage.addItem(items.front());     
   }

    Battle::fight(warrior, mage);

    return 0;
}