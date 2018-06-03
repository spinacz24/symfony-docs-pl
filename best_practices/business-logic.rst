.. index::
   double: logika biznesowa; najlepsze praktyki

Organizowanie logiki biznesowej
===============================

W oprogramowaniu komputerowym **logika biznesowa** (*ang. business logic*) lub
logika domeny (*ang. domain logic*) jest "częścią programu, która koduje rzeczywiste
procedury przetwarzania informacji w podmiotowym przedsiebiorstwie (zasady bisnesu)
lub podmiotowej domenie (np. przebieg obliczeń), ustalając jak dane mogą być tworzone
(przetwarzane), wyświetlane, przechowywane i zmieniane" (czytaj `pełną definicję`_).

Logiką biznesową, w aplikacjach Symfony, jest cały własny kod, jaki sie pisze dla
swojej aplikacji a który nie jest specyficzny dla frameworka (np. trasowanie czy
kontrolery).
Klasy domeny, encje Doctrine i zwykłe klasy PHP, które nie są używane jako usługi
sa dobrym przykładem logiki biznesowej.

Wszystko powinno sie przechowywać wewnatrz pakietu AppBundle, w większości projektów.
W pakiecie tym mozna utworzyć takie katalogi, jakie sie chce:

.. code-block:: text

    symfony2-project/
    ├─ app/
    ├─ src/
    │  └─ AppBundle/
    │     └─ Utils/
    │        └─ MyClass.php
    |─ tests/
    ├─ var/
    ├─ vendor/
    └─ web/

Czy przechowywać klasy poza pakietem?
-------------------------------------

Nie ma technicznych powodów, dla których nie można by było umieszczać logiki
biznesowej poza pakietem. Jeśli chcesz, możesz utworzyć swoją własną przestrzeń
nazw wewnątrz katalogu ``src/`` i wstawić tam swoje klasy:

.. code-block:: text

    symfony2-project/
    ├─ app/
    ├─ src/
    │  ├─ Acme/
    │  │   └─ Utils/
    │  │      └─ MyClass.php
    │  └─ AppBundle/
    ├─ tests/
    ├─ var/
    ├─ vendor/
    └─ web/

.. tip::

    Zalecanie stosowania katalogu ``AppBundle/`` jest wprowadzone dla uproszczenia
    problemu przestrzeni nazewniczych w Symfony. Zaawansowany programista, który
    wie co funkcjonuje w jego pakiecie i co może ewentualnie funkcjonować poza
    nim, może śmiało wprowadzić dodatkową przestrzeń nazewniczą.

Usługi: nazewnictwo i format
----------------------------

Aplikacja blogowa potrzebuje narzędzia, które może przekształcić tytuł wpisu
(e.g. "Hello World") do aliasu (ang. slug*) (np. "hello-world"). Ten alias będzie
wykorzystywany jako część adresu URL wpisu.

Utwórzmy nową klasę ``Slugger`` wewnatrz ``src/AppBundle/Utils/`` i dodajmy następującą
metodę ``slugify()``:

.. code-block:: php
   :linenos:

    // src/AppBundle/Utils/Slugger.php
    namespace AppBundle\Utils;

    class Slugger
    {
        public function slugify($string)
        {
            return preg_replace(
                '/[^a-z0-9]/', '-', strtolower(trim(strip_tags($string)))
            );
        }
    }

Następnie, zdefiniujmy nową usługę dla tej klasy.

.. code-block:: yaml
   :linenos:

    # app/config/services.yml
    services:
        # keep your service names short
        app.slugger:
            class: AppBundle\Utils\Slugger

Tradycyjnie, konwencja nazewnicza dla usługi odzwieciedla nazwę klasy i jej położenie,
aby uniknąć kolizji nazewniczej. Zgodnie z tym, naszą usługę *nazwalibyśmy*
``app.utils.slugger``. Jednak używając skróconych nazw usług, nasz kod stanie się
łatwiejszy do odczytania i stosowania.

.. tip::

    Nazwa usług we własnej aplikacji powinna byc jak najkrótsza, ale na tyle unikalna
    aby mozna było przeszukać projekt dla odnalezienia usługi, jeśli zajdzie taka konieczność.

Teraz można wykorzystać własna usługę slugger w dowolnej klasie kontrolera,
takiej jak ``AdminController``:

.. code-block:: php
   :linenos:

    public function createAction(Request $request)
    {
        // ...

        if ($form->isSubmitted() && $form->isValid()) {
            $slug = $this->get('app.slugger')->slugify($post->getTitle());
            $post->setSlug($slug);

            // ...
        }
    }

Format usług: YAML
------------------

W porzednim rozdziale użyliśmy formatu YAML do zdefiniowania usługi.

.. tip::

    Stosuj format YAML do definiowania własnych usług.

Jest to zasada kontrowersyjna. Według naszych obserwacji formaty YAML
i XML są równomiernie wykorzystywane przez programistów, z lekką preferencją YAML.
Obydwa formaty są tak samo wydajne, więc wybór któregoś z nich, to tylko kwestia
osobistych preferencji.

Zalecamy YAML, ponieważ jest przyjazny dla osób początkujących i zwięzły.

Usługi: brak parametru usługi
-----------------------------

Można zauważyć, że poprzednia definicja usługi nie konfiguruje przestrzeni
nazewniczej klasy jako parametr:

.. code-block:: yaml
   :linenos:

    # app/config/services.yml

    # definicja usługi z przestrzenią nazewnicza klasy jako parametr
    parameters:
        slugger.class: AppBundle\Utils\Slugger

    services:
        app.slugger:
            class: "%slugger.class%"

Praktyka ta jest uciążliwa i całkowicie zbędna dla własnych usług:

.. tip::

    Nie definiuj parametrów dla klas własnych usług.

Omawiana praktyka została blędnie przyjeta w pakietach osób trzcich. Gdy w Symfony
został wprowadzony kontener usług, niektórzy programiści stosowali tą technikę,
aby umożliwić łatwe przesłanianie usług. Jednak, przesłanianie usługi przez zmianę
jej nazwy klasy ma bardzo wąskie zastosowanie, gdyż często nowa usługa ma inne
argumenty konstruktora.

Wykorzytywanie warstwy utrwalania
---------------------------------

Symfony jest frameworkiem, który dba tylko o generowanie odpowiedzi HTTP dla
każdego żądania HTTP. Dlatego Symfony nie zapewnia sposobu na prozumienie z warstwą
utrwalania (np. bazą danych, zewnętrznym API). Można wybrać jakąkolwiek bibliotekę
lub strategię, jaką się chce.

W praktyce, wiele aplikacji Symfony wykorzystuje do definiowania swoich modeli
niezależny `projekt Doctrine`_, stosując encje i repozytoria.
Podobnie, jak w przypadku logiki biznesowej, zalecamu przechowywanie encji Doctrine
w pakiecie AppBundle.

Dobrym przykładem sa trzy encje zdefiniowane w aplikacji naszego przykładowego blogu:

.. code-block:: text

    symfony2-project/
    ├─ ...
    └─ src/
       └─ AppBundle/
          └─ Entity/
             ├─ Comment.php
             ├─ Post.php
             └─ User.php

.. tip::

    Jeśli jest się bardziej zaawansowanym, można oczywiście przechowywać je
    we własnej przestrzeni nazewniczej w katalogu ``src/``.

Informacja odwzorowania Doctrine
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Encje Doctrine sa zwyklymi obiektami PHP, które są przechowywane w jakiejś "bazie
danych". Doctrine wie tylko o swoich encjach, poprzez odwzorowanie metadanych,
skonfigurowanych dla klas modeli. Doctrine obsługuje cztery formaty metadanych:
YAML, XML, PHP i adnotacje.

.. tip::

    Używaj adnotacji do definiowania informacji odwzorowującej encje Doctrine.

Adnotacje są jak dotąd najbardziej wygodnym i błyskotliwym sposobem konfigurowania
i odszukiwania informacji odwzorowania:

.. code-block:: php
   :linenos:

    namespace AppBundle\Entity;

    use Doctrine\ORM\Mapping as ORM;
    use Doctrine\Common\Collections\ArrayCollection;

    /**
     * @ORM\Entity
     */
    class Post
    {
        const NUM_ITEMS = 10;

        /**
         * @ORM\Id
         * @ORM\GeneratedValue
         * @ORM\Column(type="integer")
         */
        private $id;

        /**
         * @ORM\Column(type="string")
         */
        private $title;

        /**
         * @ORM\Column(type="string")
         */
        private $slug;

        /**
         * @ORM\Column(type="text")
         */
        private $content;

        /**
         * @ORM\Column(type="string")
         */
        private $authorEmail;

        /**
         * @ORM\Column(type="datetime")
         */
        private $publishedAt;

        /**
         * @ORM\OneToMany(
         *      targetEntity="Comment",
         *      mappedBy="post",
         *      orphanRemoval=true
         * )
         * @ORM\OrderBy({"publishedAt" = "ASC"})
         */
        private $comments;

        public function __construct()
        {
            $this->publishedAt = new \DateTime();
            $this->comments = new ArrayCollection();
        }

        // getters and setters ...
    }

Wszystkie formaty konfiguracyjne maja tą sama wydajność, więc wybór jest znowu
tylko kwestią osobistych preferencji.

Fikstury danych
~~~~~~~~~~~~~~~

W Symfony obsługa fikstur Doctrine jest domyślnie niedostępna, tak więc
aby uzyskać do nich dostęp trzeba wykonać następujące polecenie instalujące pakiet
fikstur Doctrine:

.. code-block:: bash

    $ composer require "doctrine/doctrine-fixtures-bundle"

Następnie trzeba włączyć pakiet w ``AppKernel.php``, ale tylko dla środowisk ``dev``
i ``test``:

.. code-block:: php
   :linenos:

    use Symfony\Component\HttpKernel\Kernel;

    class AppKernel extends Kernel
    {
        public function registerBundles()
        {
            $bundles = array(
                // ...
            );

            if (in_array($this->getEnvironment(), array('dev', 'test'))) {
                // ...
                $bundles[] = new Doctrine\Bundle\FixturesBundle\DoctrineFixturesBundle();
            }

            return $bundles;
        }

        // ...
    }

Dla prostoty zalecamy utworzenie tylko *jednej* `klasy fikstury`_, chociaż
można ich utworzyć więcej, jeśli klasa ta ma dość duże obciążenie.

Zakładając, że ma sie więcej klas fikstur i że dostęp do bazy danych jest
skonfigurowany poprawnie, można załadować fikstury przez wykonanie następującego
polecenia:

.. code-block:: bash

    $ php bin/console doctrine:fixtures:load

    Careful, database will be purged. Do you want to continue Y/N ? Y
      > purging database
      > loading AppBundle\DataFixtures\ORM\LoadFixtures

Standardy kodowania
-------------------

Kod źródłowy Symfony zgodny jest ze standardami kodowania `PSR-1`_ i `PSR-2`_,
które zostały zdefiniowane przez społeczność PHP. Mozna dowiedzieć się więcej
na ten temat, czytając :doc:`the Symfony Coding standards </contributing/code/standards>`
a nawet wykorzystując `PHP-CS-Fixer`_, który jest narzędziem linii poleceń potrafiącym
doprowadzić do zgodności ze standardami cały wprowadzony kod w kilka sekund.

.. _`pełną definicję`: https://pl.wikipedia.org/wiki/Logika_biznesowa
.. _`projekt Doctrine`: http://www.doctrine-project.org/
.. _`klasy danych testowych`: https://symfony.com/doc/current/bundles/DoctrineFixturesBundle/index.html#writing-simple-fixtures
.. _`PSR-1`: http://www.php-fig.org/psr/psr-1/
.. _`PSR-2`: http://www.php-fig.org/psr/psr-2/
.. _`PHP-CS-Fixer`: https://github.com/FriendsOfPHP/PHP-CS-Fixer
