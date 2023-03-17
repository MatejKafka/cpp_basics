#include <iostream>
#include <vector>
#include <array>
#include <memory>
#include <cstdio>
#include <algorithm>

// === namespaces ==================================================================================
namespace my_library {
    class class1 {
        // ...
    };
    class class2 {
        // ...
    };
}

// ruzne zpusoby, jak muzeme instancovat tridu z namespace
void namespaces() {
    my_library::class1 c1{};

    using alias = my_library::class1;
    alias c2{};

    using my_library::class1;
    class1 c3{};

    // timhle importujeme cely namespace; casto se pouziva pro `std`, osobne to nemam rad
    using namespace my_library;
    class1 c4{};
    class2 c5{};
}


// === class definition ============================================================================
// struct je jako class, ale default modifier je public, u class je to private
struct my_struct {
    int a;
};

class my_class {
private:
    int a = 10;
    std::string b;
    std::vector<int> c;

public:
    // explicit je tu proto, ze jinak by to fungovalo jako implicitni cast
    //  (takze by nekdo mohl udelat `my_class c = vector;`) a potichu by se to precastovalo
    explicit my_class(const std::vector<int>& c) : b{"hello world"}, c(c) {}

    // dalsi constructory
    explicit my_class(const std::string& b) : b(b), c{} {}
    // bereme argumenty jako referenci, protoze jinak by se zbytecne kopirovaly 2x
    my_class(const std::string& b, const std::vector<int>& c) : b(b), c(c) {}

    // destructor, tady neni potreba, ale muzeme si v nem treba neco vypsat
    ~my_class() {
        std::cout << "my_class destructor called";
    }

    void method() {}
    // metoda, kterou muzu volat, i kdyz mam const referenci na instanci my_class
    void const_method() const {}
};

void class_instantiation_heap() {
    // alokace na heapu
    my_class* c1 = new my_class("asdf");
    // zkraceni pomoci auto
    auto* c2 = new my_class("asdf");

    // muzeme volat metody pres ->
    c1->method();

    // muzeme si vzit referenci
    auto& c1_ref = *c1;
    // a volat na ni metody
    c1_ref.method();

    // muzeme si vzit i const referenci
    const auto& c1_ref_const = *c1;
    // a volat na ni const metody
    c1_ref.const_method();

    // musime objekty na heapu zase uvolnit
    delete c1;
    delete c2;

    // pozor, ted uz nesmime pouzivat c1_ref, neodkazuje na validni objekt

    // a nebo pouzit unique_ptr, ten se uvolni na konci funkce automaticky
    std::unique_ptr<my_class> ptr = std::make_unique<my_class>("another string");
    // i tady muzeme volat pres -> metody; pres . volame metody unique_ptr, pres -> metody odkazovaneho objektu
    ptr->method();

    // a podobne s std::shared_ptr, ktery lze kopirovat a uvolni se, kdyz zanikne posledni kopie
    // muzeme si to zkratit pres auto
    auto ptr_shared1 = std::make_shared<my_class>("another string");
    // udelame kopii (tohle by nam std::unique_ptr nedovolil, protoze by pak nefungoval)
    auto ptr_shared2 = ptr_shared1;

    // obe kopie ptr_shared zaniknou na konci teto funkce, a tam se taky uvolni odkazovana instance my_class

    // muzeme na heapu alokovat jeste treba vector 3 polozek (1, 2, 3)
    auto* vec = new std::vector<int>{1, 2, 3};
    // a uvolnit ho
    delete vec;
}

void class_instantiation_stack() {
    // alokace na stacku
    // vse co tu alokujeme na stacku je uvolnene na konci teto funkce

    // preferujeme pouzivani {} misto (), protoze maji mene divnych edge cases kolem parsovani
    my_class c1{"str"};
    my_class c2{"str", std::vector<int>{1, 2, 3}};

    // pozor, tady se zkopiruje cela trida (vcetne ulozeneho vectoru), narozdil od Javy nejde jen o zmenu pointeru
    c1 = c2;

    // zase priklad s vectorem
    std::vector<std::string> vec{"hello", "world"};
}

// pokud nemam duvod kopirovat, tak std::vector chci vzdycky predavat jako referenci, a idealne jako const (pokud ho nepotrebuju upravovat)
void vectors(std::vector<int>& vec) {
    // takhle muzu pristoupit k prvnimu prvku
    if (!vec.empty()) {
        std::cout << vec[0] << "\n";
    }
    // pripadne takhle, a pokud je vector prazdny, tak dostanu exception ([] neni checked)
    std::cout << vec.at(0) << "\n";

    // ted `vec` zvetsim na 10 polozek; pokud mel doted mene jak 10 polozek, tak nove polozky jsou default-initialized (to u intu znamena 0)
    // pak muzu udelat napr. `vec[9] = 100;` a bude to validni, prepisu tim defaultni 0 na 100
    vec.resize(10);
    // timhle v bufferu vectoru zaridim misto pro 100 polozek (tedy, muzu garantovane pridat
    //  jeste 90 polozek bez toho, aby se vector musel realokovat)
    // pozor, nejsou inicializovane, jen je zarezervovane misto, a vector si mysli, ze ve zbylych
    //  90 polozkach nejsou zadna validni data; udelat ted napr. `vec[99] = 100;` by nebylo validni,
    //  ale muzeme pridavat polozky pres napr. `vec.push_back(100);`, a mame garantovano,
    //  ze nedojde jeste 90 polozek k realokaci
    vec.reserve(100);

    // timhle zapiseme do prvnich 10 prvku vectoru hodnotu 3
    std::fill_n(vec.begin(), 10, 3);
    // timhle sesortujeme cely vector
    // .begin() je iterator na prvni polozku, .end() je iterator na 1 za posledni polozkou, takze <.begin(), .end()) je cely rozsah vectoru
    std::sort(vec.begin(), vec.end());

    // muzeme iterovat pres for-range
    // pozor, tady je n kopie; kdyby ve vectoru byl nejaky komplikovanejsi typ nez int,
    //  chteli bychom spise referenci (tedy `auto&`)
    for (auto n : vec) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // bez syntax sugaru for-range se realne vykonava nasledujici for loop
    for (auto iterator = vec.begin(); iterator != vec.end(); iterator++) {
        auto n = *iterator;
        // ...
    }
}

// tohle je ukazkova templatovana funkce; zkompiluje se nam s cimkoliv, co ma definovany operator +
// tedy napr. add(1, 2)
template <typename T>
T add(const T& arg1, const T& arg2) {
    return arg1 + arg2;
}

void template_test() {
    auto r1 = add(1, 2);
    // tohle se nam nezkompiluje, protoze my_class nema operator +
    auto r2 = add(my_class{"asdf"}, my_class{"asdf"});
}

// jeste par zbylych prikladu, co se jinam nevesly
int main() {
    // std::string je fajn, pracuje se s nim vyrazne lip nez s C-style char*
    std::string str1 = "hello";
    std::string str2 = "world";
    std::string concatenated = str1 + " " + str2;

    // nacitani ze vstupu a vypis na vystup pres streamy a scanf
    unsigned long x;
    std::cout << "Hello, World!" << "\n";
    std::cin >> x;
    scanf("%lu", &x);

    // std::array je zakladni collection – je to wrapper pro C array se znamou fixni velikosti
    // narozdil od std::vector nema oddeleny buffer – kdyz array alokujete na stacku, bude opravdu cela na stacku
    // zapisuje se jako std::array<typ, pocet prvku>
    std::array<int, 3> a1{1, 2, 3};

    return 0;
}
