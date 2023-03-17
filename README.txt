C++

1. strucna historie a filozofie C++
	- make C more scalable abstraction-wise, but keep the performance
	- abstrakce musi jit obejit, pripadne by se mely kompilovat na optimalni kod
	- velke zmeny od C++98 (C with classes) do C++20 (vyrazne bezpecnejsi jazyk s higher-level abstrakcemi)
	- dusledkem evoluce C++ je, ze lze C++ psat jako vylepsene C, a to by pro vas mohl byt accessible pristup,
	  ale je lepsi se ucit moderni C++ jako vlastni jazyk
	- Java je materializovane C++ PTSD
		- Java obetovala performance v zajmu jednoduchosti (GC, vsechno je na heapu,...)

2. odkud cerpat informace
	- standard je placeny, cppreference je kind-of autoritativni zdroj, ale je to reference,
	  ne navod, MS Docs jsou dobre na pochopeni principu a kontextu
	- doporucuju CLion, dava spoustu dobrych warningu i rychlych fixu

3. includes
	- C++ headery nemaji .h priponu
	- nutne C headery jsou renamed na cstdio atd; lze pouzivat i C headery, ale je to deprecated
	- ukazat na cppreference, kde zjisti, jaky header je treba includovat (napr <iostream> na std::cout)

4. namespaces
	- v C je narocne organizovat kod, vede to na to, ze knihovny maji vsechny symboly prefixovane nazvem knihovny, coz je otravne
	- C++ zavadi namespaces: `namespace my_library {class X {};}`
	- pristup do namespace `std::cout`, `my_library::X`,...
	- namespace lze "naimportovat" pomoci `using namespace ...;`, pripadne konkretni member pomoci napr. `using std::string;`
	- jeste existuje `using alias = original_name;`, moderni alternativa typedef, trochu lip se cte a funguje s templaty
		- btw, tohle je duvod, proc jsou v chybovych hlaskach a cppreference dokumentaci obcas zvlastni typy jako napr.
		  `std::basic_string<T, ...>` misto `std::string` – `std::string` je jen typedef za obecnejsi typ `std::string`
		  s konkretnimi template argumenty
	- namespaces nejsou vazane na compilation units, ani na soubory
	- namespace standardni knihovny: `std::`

5. IO streams
	- spis se jim vyhybejte, jsou pomale, maji zvlastni stateful chovani a divnou syntaxi
	- pouzivaji se tak, ze do streamu pres << strkate objekty, co maji overloadovany operator,
	  pripadne ze streamu pres >> objekty parsujete
	- vyhodou oproti printf/scanf je, ze jsou polymorficke, takze kdyz zmenite typ promenne, do ktere nacitate
	  (napr. z intu na long nebo string), tak se prizpusobi i nacitani
	- taky lze streamy pouzivat napr. u string builderu (`std::stringstream`) pro skladani stringu v pameti

6. reference vs pointery
	- referenci nelze prebindovat (jakmile referenci vytvorim, uz vzdy pointuje na dany objekt, nelze zmenit cilovy objekt)
	- reference je transparentni (netreba pouzivat -> a * pro dereferenci)
		- assignment prepise cilovou hodnotu
		  napr. `int n; int& n_ref = n; n = 10;` zapise 10 do `n`
	- reference se pouzivaji nejcasteji pro predavani argumentu do funkce by-reference (misto kopirovani)
	- pointer na reference lze konvertovat pomoci * (`type& ref = *ptr;`), naopak pomoci `type* ptr = &ref`
		- C++ pozna, ze vysledkem ma byt reference, takze dereference nevede na zkopirovani objektu
	- pointer muze byt `nullptr` (pozor, zmena z NULL), reference musi pointovat na valid objekt (sans lifetime issues)

7. deklarace trid, class vs struct, visibility, members
	- constructor (+ member initializers, i inline), destructor (ale ne move/copy constructory atd)
		- default constructor (dulezite protoze v C++ nesmi byt uninitialized pamet)
		- implicit/explicit single-parameter constructor
	- operator overloading – https://www.geeksforgeeks.org/operator-overloading-cpp/
	- na zaver: jak se chova const u trid?
		- u const reference lze volat jen const-annotated metody, a ty nesmi v objektu nic menit
		- narozdil od Javy se projevuje nejen na referenci, ale na jeji target, tedy napr. neni mozne menit obsah const vectoru

8. RAII
	- myslenka: pokud nase trida spravuje nejake zdroje (buffer na heapu, mutex, file descriptor, OS vlakno),
	  tak zdroj ziskame v constructoru (alokace, fopen, mutex.lock(),...), a v destructoru ho uvolnime
	- examples: https://en.cppreference.com/w/cpp/language/raii
	- examples: std::string, std::unique_lock, std::ofstream, std::unique_ptr, std::jthread
	- resi exception-safety, multiple returns, exceptions pri inicializaci tridy,... (narozdil od Javy nas nezachrani GC)
		- narozdil od Javy C++ nepotrebuje try/finally, pokud se spravne pouziva RAII

9. instanciace trid, stack vs heap
	- pokud chci udelat instanci na stacku, udelam `type varname(arg1, arg2);` nebo `type varname{arg1, arg2}`
		- pozor na `type varname();`, to je deklarace funkce, chcete `type varname{};`
		- u kolekci jako napr std::vector je rozdil; `std::vector<int> v(10)` vytvori vector 10 defaultnich polozek,
		  `std::vector<int> v{10}` vytvori vector o 1 polozce s hodnotou 10
		- jinak preferujte pouzivani {} pred (), je predvidatelnejsi
		- pozor na lifetime – objekt na stacku je uvolneny kdyz skonci funkce, kde byl definovany,
		  reference na takovy objekt by nemely prezit dele nez samotny objekt, jinak use-after-free
		- pozor, ze narozdil od Javy assignovani do hodnoty na stacku actually prepisuje ten objekt, nejde jen o zmenu pointeru,
		  takze to muze byt pomale
	- alokace na heapu:
		- vraci obycejne C pointery
		- `int* i_ptr = new int(10);`
		- `std::vector<int>* vec_ptr = new std::vector<int>{1, 2, 3};`
		- opakovani typu lze zjednodusit pomocit `auto`: `auto* vec_ptr = new std::vector<int>{1, 2, 3};`
		- pak treba manualne uvolnit pomoci `delete vec_ptr;`
			- pozor, nesmi se mixovat malloc/free a new/delete, zapomente ze malloc/free existuji
	- pokud muzete, chcete alokovat na stacku, ale pozor na leakujici reference atd
		- nikdy neni dobre udelat na zacatku funkce new a na konci delete, radsi alokujte na stacku
	- heap je typicky potreba na polymorfismus (objekty maji ruznou velikost) a pokud chci velky objekt hodne predavat
	  mezi funkcemi a nenapada me zadna funkce, jejiz scope bude existovat dele nez dany objekt, kde bych objekt alokoval na stacku
	- s normalnimi pointery je problem, ze je snadne je omylem leakovat (napr. early return)
	- reseni jsou smart pointers:
		- https://learn.microsoft.com/en-us/cpp/cpp/smart-pointers-modern-cpp?view=msvc-170
		- `auto unique = std::make_unique<int>(10);`
		- `auto shared = std::make_shared<int>(10);`
		- unique_ptr vs shared_ptr
			- unique_ptr = vlastni objekt na heapu, kdyz zanikne, tak dealokuje cilovy objekt
				- pouziva se, protoze unique_ptr je rychlejsi predavat nez kopirovat/presouvat samotny objekt, ale je tam overhead pri pristupu
				  jine usecases: https://stackoverflow.com/questions/26071862/when-to-use-unique-ptr-instead-of-local-object
			- shared_ptr = u objektu je i reference counter, pri kazde kopii shared_ptr se incrementuje a destructor kazde kopie ho decrementuje,
			  kdyz se dostane counter na nulu, tak je cilovy objekt dealokovan
			  	- pouziva se nejcasteji, kdyz potrebuju predavat objekt do vice vlaken, nebo mit referenci ve vice tridach,
			  	  a nedokazu vymyslet jeden scope, ktery urcite prezije vsechny reference (caste v multithreaded programovani
			  	  a GUI frameworcich jako Qt)

10. move vs copy
	- copy = zkopiruju objekt vcetne vsech resources
	- move = vykradu resources puvodni kopie objektu, uz bych puvodni objekt nemel pouzivat
		- napr. u std::vector muzu ukrast buffer na heapu
		- napr. u std::thread musim pouzit vzdy move, protoze kopirovat std::thread nedava smysl
		- move napr. pouziva interne std::vector, kdyz realokuje svuj buffer pri zvetsovani
	- diagram s std::vector a heap storage
	- default je copy, pokud nechcete kopirovat, musite udelat bud std::move, nebo predavat literal, nebo predavat referenci
		- pozor, move je ozehavy, dukladne si prectete, jak se chova, nez ho zacnete vic pouzivat, idealne to chce pochopit lvalues vs rvalues, && a implementaci std::move
		- nicmene v nekterych jednoduchych pripadech (napr. vector.push_back(std::thread(...))) to zadarmo funguje a neni treba nic resit
	- bonus (nestihli jsme):
		- copy/move constructor, copy/move assignment operators
		- ownership, lvalues vs rvalues

11. templates
	- `template <typename T, size_t N>` – 2 template argumenty, prvni je typ, druhy je compile-time konstanta
		- takhle vypada napr. `std::array<T, N>`
	- predstavte si, ze se vezme definice tridy/funkce, a copy-pastuje se, ale s vyplnenym templatem
		- takhle se to i kompiluje
		- proc se to pouziva? protoze je to rychlejsi a vic hackable nez generiky
	- narozdil od Javy, ktera ma interface, u C++ se typ dosadi a pak se kontroluje, jestli sedi typy v interni implementaci templatu
		-> hrozive error messages
			- ukazat priklad kolegy, co zkousel udelat std::random_shuffle s const vectorem
	- pokud zbude cas:
		- constexpr, jak to interaguje s templates

12. uzitecne container tridy (nestihli jsme vsechno)
	- std::string
	- std::array<T, N> – no overhead nahrada C array, co lip hraje se zbytkem C++ (ma .size(), iteratory,...)
	- std::vector<T>
		.reserve = rezervuj misto na heapu
		.resize = zmen velikost vectoru na danou velikost a default-inicializuj (prazdny constructor) nove polozky
	- std::map<K, V>
	- std::unordered_map<K, V>
	- std::list<T> – typicky double linked list
	- for-range, ukazat, jak funguje iterace, a proc je nekdy efektivnejsi nez for loop s indexy
		- .begin(), .end()
		- implementace for-range:
			`for (auto& val_ref : collection) {...}`
			`for (auto iterator = collection.begin(); iterator != collection.end(); iterator++) {auto& val_ref = *iterator; ...}`
	- std::vector .push_back vs .emplace_back
		- push_back zkopiruje nebo presune (copy, move) argument do vectoru
		- emplace_back zavola constructor typu s predanymi argumenty rovnou v bufferu vectoru