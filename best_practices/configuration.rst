Konfiguracja
============

Konfiguracja obejmuje zwykle różne części aplikacji (takie jak infrastruktura
i poświadczenia bezpieczeństwa) oraz różne środowiska (programistyczne, produkcyjne).
Dlatego zaleca się, aby w Symfony podzielić konfigurację aplikacji na trzy części.

.. _config-parameters.yml:

Konfiguracja związana z infrastrukturą
--------------------------------------

.. tip::

    Definiuj opcje konfiguracji związanej z infrastrukturą w pliku
    ``app/config/parameters.yml`` file.

Domyślny plik ``parameters.yml`` zgodny jest z naszym zaleceniem i definiuje opcje
odnoszace sie do infrastruktury bazy danych i serwera pocztowego:

.. code-block:: yaml

    # app/config/parameters.yml
    parameters:
        database_driver:   pdo_mysql
        database_host:     127.0.0.1
        database_port:     ~
        database_name:     symfony
        database_user:     root
        database_password: ~

        mailer_transport:  smtp
        mailer_host:       127.0.0.1
        mailer_user:       ~
        mailer_password:   ~

        # ...

Opcje te nie są zdefiniowane wewnątrz pliku ``app/config/config.yml`` ponieważ
nie maja nic wspólnego z zachowaniem aplikacji. Unnymi słowami, aplikacja nie
zajmuje się lokalizacją bazy danych lub poswiadczeniami dostępu do niej,
pod warunkiem, że jest ona dobrze skofigurowana.

.. _best-practices-canonical-parameters:

Parametry kanoniczne
~~~~~~~~~~~~~~~~~~~~

.. tip::

    Definiuj wszystkie parametry aplikacji w pliku
    ``app/config/parameters.yml.dist``.

Symfony zawiera, od wersji 2.3, plik konfiguracyjny o nazwie ``parameters.yml.dist``,
który przechowuje listę parametrów kanonicznych dla aplikacji.

Gdy dla aplikacji zostaje zdefiniowany nowy parametr konfiguracyjny, powinno się
go również dodać do tego pliku i zgłosoć zmiany do swojego systemu kontroli wersji.
Następnie, gdy programista dokona aktualizacji projektu lub jego wdrożenia na
serwerze, Symfony sprawdzi, czy istnieja różnice pomiędzy kanonicznym plikiem
``parameters.yml.dist`` a lokalnym plikiem ``parameters.yml``. Gdy takie różnice
zostaną stwierdzone, Symfony poprosi o podanie wartości dla nowych parametrów
i doda je do lokalnego pliku ``parameters.yml``.

Konfiguracja związana z aplikacją
---------------------------------

.. tip::

    Definiuj opcje konfiguracyjne związanez zachowaniem aplikacji w pliku
    ``app/config/config.yml`.

Plik ``config.yml`` zawiera opcje używane przez aplikację do zmieniania jej 
zachowania, takich jak wysyłanie powiadomień email czy udostępnianie
`przełączania funkcji`_. Zdefiniowanie tych wartości w pliku ``parameters.yml``
tworzyło by dodatkową warstwę konfiguracyjną, która nie byłaby potrzebna, bo
przecież nie potrzebujesz i nie chcesz konfigurować wartosci przy każdej zmianie
serwera.

Opcje konfiguracyjne zdefiniowane w pliku ``config.yml``, zwykle dotyczą
jednego :doc:`środowiska </cookbook/configuration/environments>`, choć
w rzeczywistości można ich wykorzystywać więcej. Jest tak dlatego, że Symfony
zawiera już pliki ``app/config/config_dev.yml`` i ``app/config/config_prod.yml``,
które przesłaniają określone wartości dla każdego z tych środowisk.

Stałe vs opcje konfiguracyjne
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jednym z najczęstszych błędów popełnianych podczas definiowania konfiguracji
aplikacji jest tworzenie nowych opcji dla wartości, które nigdy sie nie zmienią,
jak liczba elementów dla stronicowanych wyników.

.. tip::

    używaj stałych do definiowania opcji konfiguracyjnych, które rzedko się
    zmieniają.

Tradycyjne podejście do okeślania opcji konfiguracyjnych powoduje, ze wiele aplikacji
Symfony dołączaja opcje taka jak pokazano niżej, które mogłyby być wykorzystane
do kontrolowania ilości wpisów do wyświetlania na stronie początkowej bloga:

.. code-block:: yaml

    # app/config/config.yml
    parameters:
        homepage.num_items: 10

Ta opcja rzadko się będzie zmieniać, jeśli w ogóle. Tworzenie tego typu opcji
konfiguracyjnych jest zbedne.
Zalecamy definiowanie takich wartości w aplikacji jako stałych.
Można, na przykład, zdefiniowac stałą ``NUM_ITEMS`` w encji ``Post``:

.. code-block:: php

    // src/AppBundle/Entity/Post.php
    namespace AppBundle\Entity;

    class Post
    {
        const NUM_ITEMS = 10;

        // ...
    }

Główna korzyścią płynacą z używania stałych jest to, że można je używać wszędzie
w aplkiacji, gdy natomiast parametry są dostępne tylko w miejscach, z których
jest dostęp do kontenera Symfony.

Stałe mogą być uzywane, na przykład, w szablonach Twig dzięki 
`funkcji constant()`_:

.. code-block:: html+jinja

    <p>
        Displaying the {{ constant('NUM_ITEMS', post) }} most recent results.
    </p>

Takze encje i repozytoria Doctrine moga teraz uzyskać łatwo dostęp do tych wartosci,
podczas gdy nie mogą uzyskać dostpu do parametrów kontenera:

.. code-block:: php

    namespace AppBundle\Repository;

    use Doctrine\ORM\EntityRepository;
    use AppBundle\Entity\Post;

    class PostRepository extends EntityRepository
    {
        public function findLatest($limit = Post::NUM_ITEMS)
        {
            // ...
        }
    }

Jedyną znaczacą wadą stosowania stałych konfiguracyjnych jest to, że nie można
ich łatwo przedefiniowywać w testach.

Semantyczna konfiguracja: nie rób tego
--------------------------------------

.. tip::

    Nie określaj w pakietach sematycznej konfiguracji dla wstrzykiwania zależności.

Tak jak wyjaśniono to w artykule :doc:`/cookbook/bundles/extension`, pakiety
Symfony maja dwie możliwości obsługi konfiguracji: zwykłą obsługę konfiguracji
poprzez plik ``services.yml`` i sematyczna konfigurację poprzez  specjalną klasę
``*Extension``.

Chociaż sematyczna konfiguracja jest znacznie bardziej zaawansowana i dostarcza
ciekawych możliwosci, takich jak walidację konfiguracji, to jednak nakład
pracy potrzebny do zdefiniowania takiej konfiguracji jest zbyt duży i zbędny
w pakietach, które nie są przeznaczone do rozpowszechniania.

Przenoszenie wrażliwych opcji całkowicie poza Symfony
-----------------------------------------------------

Gdy mamy do czynienia z opcjami wrażliwymi, takimi jak poświadczenia bazy danych,
lepiej je przechowywać poza aplikacją Symfony i wykonać dostęp do nich poprzez
zmienne środowiskowe. Jak to zrocić? Prosze przeczytac artykuł:
:doc:`/cookbook/configuration/external_parameters`

.. _`przełączania funkcji`: https://en.wikipedia.org/wiki/Feature_toggle
.. _`funkcji constant()`: http://twig.sensiolabs.org/doc/functions/constant.html
