#include <iostream>
#include <vector>
#include <cstring>
#include <unordered_set>
#include <map>
#include <string>

#include "hash.h"

namespace util {

    const std::string sequence_to_string(uint64_t const * seq, size_t size) {
        
        if (seq == NULL || size == 0) {
            return "NULL";
        }
        
        std::string sequence_string = "\"";
        for (size_t i = 0; i < size;i++) {
            sequence_string += std::to_string(seq[i]);
            if (i != size - 1) sequence_string += " ";
        }
        sequence_string += "\"";
        return sequence_string;
    }

}

namespace jnp1 {

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

            for (size_t i = 0; i < size; i++)
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


    std::map<unsigned long, std::unordered_set<Sequence, Hasher>>& get_sets() {
        static auto* sets = new std::map<unsigned long, std::unordered_set<Sequence, Hasher>>();
        return *sets;
    }

    #define sets get_sets()

    //all sets are stored in set, indexed by order in which they were inserted
    //std::map<unsigned long, std::unordered_set<Sequence, Hasher>> sets;
    unsigned long set_counter = 0;

    /*  Tworzy tablicę haszującą i zwraca jej identyfikator. Parametr
            hash_function jest wskaźnikiem na funkcję haszującą, która daje w wyniku
    liczbę uint64_t i ma kolejno parametry uint64_t const * oraz size_t.  */
    unsigned long hash_create(hash_function_t hash_function) {

        std::cerr << "hash_create(" << hash_function << ")" << std::endl;

        if (hash_function == NULL) {
            std::cerr << "hash_create: invalid hash_function (NULL)" << std::endl;
            return 0;
        }

        set_counter++;
        std::unordered_set<Sequence, Hasher> set(0, Hasher(hash_function));

        sets.emplace(set_counter, set);
        std::cerr << "hash_create: hash table #" << set_counter << " created" << std::endl;
        return set_counter;
    }

    /*Usuwa tablicę haszującą o identyfikatorze id, o ile ona istnieje.
    W przeciwnym przypadku nic nie robi. */
    void hash_delete(unsigned long id) {
        std::cerr << "hash_delete(" << id << ")" << std::endl;
        sets.erase(id);
    }

    /*Daje liczbę ciągów przechowywanych w tablicy haszującej
            o identyfikatorze id lub 0, jeśli taka tablica nie istnieje. */
    size_t hash_size(unsigned long id) {

        std::cerr << "hash_size(" << id << ")" << std::endl;

        auto set_iter = sets.find(id);

        if (set_iter == sets.end()) {
            std::cerr << "hash_size: hash table #" << id << " doesn't exist" << std::endl;
            return 0;
        }

        std::unordered_set<Sequence, Hasher> &set = set_iter->second;

        size_t size = set.size(); 
        std::cerr << "hash_size: hash table #" << id << " contains " << size << " elements" << std::endl;
        return size;
    }

    /*Wstawia do tablicy haszującej o identyfikatorze id ciąg liczb
            całkowitych seq o długości size. Wynikiem jest informacja, czy operacja
    się powiodła. Operacja się nie powiedzie, jeśli nie ma takiej tablicy
    haszującej, jeśli tablica haszująca zawiera już taki ciąg, jeśli
            parametr seq ma wartość NULL lub parametr size ma wartość 0. */
    bool hash_insert(unsigned long id, uint64_t const * seq, size_t size) {
        std::cerr << "hash_insert(" << id << ", " << util::sequence_to_string(seq, size) << ", " << size << ")" << std::endl;
        
        if (seq == NULL) {
            std::cerr << "hash_insert: invalid pointer (NULL)" << std::endl;
            return false;
        }
        if (size == 0) {
            std::cerr << "hash_insert: invalid size (0)" << std::endl;
            return false;
        }

        auto set_iter = sets.find(id);

        if (set_iter == sets.end()) {
            std::cerr << "hash_size: hash table #" << id << " doesn't exist" << std::endl;
            return false;
        }

        Sequence s(seq, size);
        std::unordered_set<Sequence, Hasher> &set = set_iter->second;

        if (set.find(s) != set.end()) {
            std::cerr << "hash_insert: hash table #" << id << " already contains sequence " << util::sequence_to_string(seq, size) << std::endl;
            return false;
        }

        set.insert(s);
        std::cerr << "hash_insert: hash table #" << id <<", sequence " << util::sequence_to_string(seq, size) << " inserted" << std::endl;
        return true;
    }

    /*Usuwa z tablicy haszującej o identyfikatorze id ciąg liczb całkowitych
    seq o długości size. Wynikiem jest informacja, czy operacja się
            powiodła. Operacja się nie powiedzie, jeśli nie ma takiej tablicy
    haszującej, jeśli tablica haszująca nie zawiera takiego ciągu,
    jeśli parametr seq ma wartość NULL lub parametr size ma wartość 0. */
    bool hash_remove(unsigned long id, uint64_t const * seq, size_t size){
        std::cerr << "hash_remove(" << id << ", " << util::sequence_to_string(seq, size) << ", " << size << ")" << std::endl;

        if (seq == NULL) {
            std::cerr << "hash_remove: invalid pointer (NULL)" << std::endl;
            return false;
        }
        if (size == 0) {
            std::cerr << "hash_remove: invalid size (0)" << std::endl;
            return false;
        }

        auto set_iter = sets.find(id);

        if (set_iter == sets.end()) {
            std::cerr << "hash_remove: hash table #" << id << " doesn't exist" << std::endl;
            return false;
        }

        Sequence s(seq, size);
        std::unordered_set<Sequence, Hasher> &set = set_iter->second;

        if (set.find(s) == set.end()) {
            std::cerr << "hash_remove: hash table #" << id << " already contains sequence " << util::sequence_to_string(seq, size) << std::endl;
            return false;
        }

        set.erase(s);
        std::cerr << "hash_remove: hash table #" << id <<", sequence " << util::sequence_to_string(seq, size) << " removed" << std::endl;
        return true;
    }

    /*Jeśli tablica haszująca o identyfikatorze id istnieje i nie jest pusta,
    to usuwa z niej wszystkie elementy. W przeciwnym przypadku nic nie robi. */
    void hash_clear(unsigned long id) {
        std::cerr << "hash_clear(" << id << ")" << std::endl;
        auto set_iter = sets.find(id);

        if (set_iter == sets.end()) {
            std::cerr << "hash_remove: hash table #" << id << " doesn't exist" << std::endl;
            return;
        }

        std::unordered_set<Sequence, Hasher> &set = set_iter->second;
        set.clear();
    }

    /*Daje wynik true, jeśli istnieje tablica haszująca o identyfikatorze id
            i zawiera ona ciąg liczb całkowitych seq o długości size. Daje wynik
    false w przeciwnym przypadku oraz gdy parametr seq ma wartość NULL lub
            parametr size ma wartość 0. */
    bool hash_test(unsigned long id, uint64_t const * seq, size_t size) {
        std::cerr << "hash_test(" << id << ", " << util::sequence_to_string(seq, size) << ", " << size << ")" << std::endl;

        if (seq == NULL) {
            std::cerr << "hash_test: invalid pointer (NULL)" << std::endl;
            return false;
        }
        if (size == 0) {
            std::cerr << "hash_test: invalid size (0)" << std::endl;
            return false;
        }

        auto set_iter = sets.find(id);

        if (set_iter == sets.end()) {
            std::cerr << "hash_test: hash table #" << id << " doesn't exist" << std::endl;
            return false;
        }

        Sequence s(seq, size);
        std::unordered_set<Sequence, Hasher> &set = set_iter->second;

        bool contains = set.find(s) != set.end(); 
        std::cerr << "hash_test: hash table #" << id << ", sequence " << util::sequence_to_string(seq, size) << (contains ? " is present" : " is not present") << std::endl;
        return contains;
    }
}
