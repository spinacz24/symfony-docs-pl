.. index::
   double: testy; najlepsze praktyki

Testy
=====

Ogólnie rzecz biorąc, istnieją dwa rodzaje testów. Testy jednostkowe pozwalają
testować wejście i wyjście określonych funkcji. Testy funkcjonalne umożliwiają
kierować "przeglądarką", gdzie można przeglądać strony na swojej witrynie, klikać
odnośniki, wypełniać formularze i stwierdzać, czy elementy na stronie działają
lub są rozmieszczone prawidłowo.

Testy jednostkowe
-----------------

Testy jednostkowe są wykorzystywane do testowania "logiki biznesowej" i powinny
być umieszczane w klasach, które są niezależne od Symfony. Z tego powodu, Symfony
tak na prawdę nie określa, jakie narzędzia trzeba stosować dla testów jednostkowych.
Jednak, najczęściej używanymi narzędziami są `PhpUnit`_ i `PhpSpec`_.

Testy funkcjonalne
------------------

Tworzenie na prawdę dobrych testów funkcjonalnych może być trudne, więc wielu
programistów je całkowicie pomija. Nie należy pomojać testów funkcjonalnych!
Definiując nawet nieco uproszczone testy funkcjonalne, można szybko zauważyć
większe błędy jeszcze na etapie testów:

.. best-practice::

    Zdefinuj test funkcjonalny, który co najmniej sprawdza, czy strony aplikacji
    są pomyślnie ładowane.

Test funkcjonalny może być łatwy, jak to:

.. code-block:: php
   :linenos:

    // tests/AppBundle/ApplicationAvailabilityFunctionalTest.php
    namespace Tests\AppBundle;

    use Symfony\Bundle\FrameworkBundle\Test\WebTestCase;

    class ApplicationAvailabilityFunctionalTest extends WebTestCase
    {
        /**
         * @dataProvider urlProvider
         */
        public function testPageIsSuccessful($url)
        {
            $client = self::createClient();
            $client->request('GET', $url);

            $this->assertTrue($client->getResponse()->isSuccessful());
        }

        public function urlProvider()
        {
            return array(
                array('/'),
                array('/posts'),
                array('/post/fixture-post-1'),
                array('/blog/category/fixture-category'),
                array('/archives'),
                // ...
            );
        }
    }

Kod ten sprawdza, czy dla wszystkich podanych ścieżek URL ładowane są poprawnie
strony, co oznacza, że ich kod statusu HTTP mieści się w przedziale pomiędzy
``200`` a ``299``. Nie wygląda to może przydatnie, ale biorąc pod uwagę, jak
niewiele wysiłku zostało włożone, warto coś takiego wykonać w swojej aplikacji.

W programowaniu komuterowym, ten rodzaj testu nosi nazwę `smoke testing`_ i zwiera
*"wstępne testy ujawniajace proste błędy, na tyle poważne, aby odrzucić przedkładaną
wersję oprogramowania"*.

Sztywne ścieżki URL w teście funkcjonalnym
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Niektórzy Czytelnicy mogą mieć wątpliwości, dlaczego w poprzednim teście funkcjonalnym
nie wykorzystano usługi generatora URL:

.. best-practice::

    Sztywno koduj ścieżki URL w testach funkcjonalnych, zamiast używać generatora URL.

Rozważmy nastęþujacy test funkcjonalny, który używa usługi ``router`` do wygenerowania
ścieżki URL testowanej strony:

.. code-block:: php
   :linenos:

    public function testBlogArchives()
    {
        $client = self::createClient();
        $url = $client->getContainer()->get('router')->generate('blog_archives');
        $client->request('GET', $url);

        // ...
    }

To działa, ale ma jedną *potężną* wadę. Jeśli programista przez pomyłkę zmieni
ścieżkę trasy ``blog_archives``, test w dalszym ciągu będzie przechodził, ale
oryginalna (stara) ścieżka URL nie będzie działać! Oznacza to, że zostana utracone
wszystkie zakładki dla tej ścieżki i straci się ranking wyszukiwania tej strony.

Testowanie funkcjonalnosci JavaScript
-------------------------------------

Wbudowany w Symfony :ref:`klient testowania funkcjonalnego <book_testing_client>`
jest świetny, ale nie może być zastosowany do testowania zachowań opartych na
kodzie JavaScript na stronach. Jeśli zachodzi potrzeba przetestowania tego,
trzeba rozważyć wykorzystanie biblioteki `Mink`_ z PHPUnit.

Oczywiście, jeśli ma się "ciężki frontend" oparty na JavaScript, powinno się wziąść
pod uwagę narzędzia testowania oparte na czystym JavaScript.

Więcej informacji o testach funkcjonalnych
------------------------------------------

Proszę rozważyć biblioteki `Faker`_ i `Alice`_, umożliwiające generowanie
pseudo-rzeczywistych danych dla fikstur testowych.

.. _`Faker`: https://github.com/fzaninotto/Faker
.. _`Alice`: https://github.com/nelmio/alice
.. _`PhpUnit`: https://phpunit.de/
.. _`PhpSpec`: http://www.phpspec.net/
.. _`Mink`: http://mink.behat.org
.. _`smoke testing`: https://en.wikipedia.org/wiki/Smoke_testing_(software)
