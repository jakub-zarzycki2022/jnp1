#include <iostream>
#include <vector>
#include <cstring>
#include <unordered_set>
#include <map>

namespace hash{
    typedef uint64_t (*hash_function_t) (uint64_t const *, size_t);

    class Sequence {
    public:
        std::vector<uint64_t> data;
        size_t size;

        Sequence(uint64_t const * _data, size_t _size) {
            this->size = _size;
            data = std::vector<uint64_t> (_data, _data + size);
        };

        bool operator==(const Sequence &other) const {
            if (this->size != other.size)
                return false;

            for (int i = 0; i < size; i++)
                if (this->data[i] != other.data[i])
                    return false;

            return true;
        }
    };

    class Hasher {
    public:
        hash_function_t hash_function;

        explicit Hasher (hash_function_t _hash_function) {
            hash_function = _hash_function;
        }

        std::size_t operator()(const Sequence& key) const {
            return hash_function(key.data.data(), key.size);
        }
    };

    //all sets are stored in set, indexed by order in which they were inserted
    std::map<unsigned long, std::unordered_set<Sequence, Hasher>> sets;
    unsigned long set_counter = 0;

    /*  Tworzy tablicę haszującą i zwraca jej identyfikator. Parametr
            hash_function jest wskaźnikiem na funkcję haszującą, która daje w wyniku
    liczbę uint64_t i ma kolejno parametry uint64_t const * oraz size_t.  */
    unsigned long hash_create(hash_function_t hash_function) {
        set_counter++;
        std::unordered_set<Sequence, Hasher> set(0, Hasher(hash_function));

        sets.emplace(set_counter, set);
        return set_counter;
    }

    /*Usuwa tablicę haszującą o identyfikatorze id, o ile ona istnieje.
    W przeciwnym przypadku nic nie robi. */
    void hash_delete(unsigned long id) {
        sets.erase(id);
    }

    /*Daje liczbę ciągów przechowywanych w tablicy haszującej
            o identyfikatorze id lub 0, jeśli taka tablica nie istnieje. */
    size_t hash_size(unsigned long id) {
        std::unordered_set<Sequence, Hasher> &set = sets.find(id)->second;
        return set.size();
    }

    /*Wstawia do tablicy haszującej o identyfikatorze id ciąg liczb
            całkowitych seq o długości size. Wynikiem jest informacja, czy operacja
    się powiodła. Operacja się nie powiedzie, jeśli nie ma takiej tablicy
    haszującej, jeśli tablica haszująca zawiera już taki ciąg, jeśli
            parametr seq ma wartość NULL lub parametr size ma wartość 0. */
    bool hash_insert(unsigned long id, uint64_t const * seq, size_t size) {
        Sequence s(seq, size);
        std::unordered_set<Sequence, Hasher> &set = sets.find(id)->second;
        set.insert(s);

        return true;
    }

    /*Usuwa z tablicy haszującej o identyfikatorze id ciąg liczb całkowitych
    seq o długości size. Wynikiem jest informacja, czy operacja się
            powiodła. Operacja się nie powiedzie, jeśli nie ma takiej tablicy
    haszującej, jeśli tablica haszująca nie zawiera takiego ciągu,
    jeśli parametr seq ma wartość NULL lub parametr size ma wartość 0. */
    bool hash_remove(unsigned long id, uint64_t const * seq, size_t size){
        Sequence s(seq, size);
        std::unordered_set<Sequence, Hasher> &set = sets.find(id)->second;

        set.erase(s);

        return true;
    }

    /*Jeśli tablica haszująca o identyfikatorze id istnieje i nie jest pusta,
    to usuwa z niej wszystkie elementy. W przeciwnym przypadku nic nie robi. */
    void hash_clear(unsigned long id) {
        std::unordered_set<Sequence, Hasher> &set = sets.find(id)->second;
        set.clear();
    }

    /*Daje wynik true, jeśli istnieje tablica haszująca o identyfikatorze id
            i zawiera ona ciąg liczb całkowitych seq o długości size. Daje wynik
    false w przeciwnym przypadku oraz gdy parametr seq ma wartość NULL lub
            parametr size ma wartość 0. */
    bool hash_test(unsigned long id, uint64_t const * seq, size_t size) {
        Sequence s(seq, size);
        std::unordered_set<Sequence, Hasher> &set = sets.find(id)->second;
        return set.contains(s);
    }
}
