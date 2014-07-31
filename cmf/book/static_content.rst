.. index::
    single: treść statyczna; podręcznik
    single: CmfContentBundle

Treść statyczna
===============

Koncepcja
---------

Sercem każdego CMS jest treść, abstrakcja, którą wydawcy mogą manipulować i prezentować
później na stronach. Struktura treści w dużej mierze zależy od projektu i to ma
istotny wpływ na przyszłe rozwiązania i wykorzystanie platformy.

Pakiet ContentBundle dostarcza podstawową implementację klas dokumentów treści,
w tym obsługę wielojęzyczności i powiązań z trasami.

Klasa StaticContent
-------------------

Klasa ``StaticContent`` deklaruje podstawową strukturę treści. Struktura ta jest
bardzo podobna do struktury stosowanej w systemach ORM Symfony2. Większość jej
pól nie wymaga wyjaśnień i są takie, jak oczekuje się w podstawowym CMS:
tytuł, treść, informacje o publikowaniu i odniesienie od dokumentu nadrzędnego,
aby umieścić dokument w drzewiastej strukturze. Zawiera ona również referencje
bloku (więcej o tym dalej).

Ta klasa dokumentu implementuje trzy interfejsy udostępniające dodatkowe funkcjonalności:

* ``RouteReferrersInterface`` oznacza, ze treść jest powiązana z trasami;
* ``PublishTimePeriodInterface`` oznacza, że treść ma daty publikacji i wycofania
  publikacji, które będą obsługiwane przez rdzeń Symfony CMF do określania, czy
  treść ma być wyświetlana przez ``StaticContent``;
* ``PublishableInterface`` oznacza, że treść ma flagę logiczna, która będzie obsługiwana
  przez rdzeń Symfony CMF do określania, czy treść ma być wyświetlana przez ``StaticContent``.

Kontroler treści
----------------

Zawarty jest również kontroler, który może renderować różne rodzaje dokumentów.
Jego jedyna akcja, ``indexAction``, akceptuje instancję treści i opcjonalnie ścieżkę
szablonu do renderowania. Jeśli nie zostanie dostarczona ścieżka szablonu, użyty
zostanie szablon określony w konfiguracji jako domyślny.

Akcja kontrolera pobiera również stan publikowanych dokumentów i język (dla
``MultilangStaticContent``). Zarówno instancja treści jak i opcjonalny szablon są
dostarczane do kontrolera przez ``DynamicRouter`` pakietu RoutingBundle. Więcej
informacji na ten temat dostępnych jest na :ref:`stronie początkowej pobierania
systemu trasowania <start-routing-linking-a-route-with-a-model-instance>`.

Obsługa interfejsu administracyjnego
------------------------------------

Ostatni komponent potrzebny do obsługi dołączonych rodzajów treści, to panel administracyjny.
Symfony CMF może opcjonalnie obsługiwać SonataDoctrinePHPCRAdminBundle_, narzędzia
generacji back office.

W pakiecie ContentBundle wymagane panele administracyjne są już zadeklarowane
w folderze ``Admin`` i skonfigurowane w pliku ``Resources/config/admin.xml``
i zostaną automatycznie załadowane, jeśli zainstalowany jest pakiet
SonataDoctrinePHPCRAdminBundle (instrukcja podana jest w artykule
:doc:`../cookbook/creating_a_cms/sonata-admin`).

Konfiguracja
------------

ContentBundle obsługuje również zestaw opcjonalnych parametrów konfiguracyjnych.
W celu zapoznania się z pełną informacją o konfiguracji prosimy o zapoznanie się
z :doc:`../bundles/content/index`.

Wnioski końcowe
---------------

Chociaż ten mały pakiet zawiera kilka istotnych komponentów do pełnego działania
CMS, to często brak w nim czegoś co jest potrzebne. Główna ideą tego pakietu jest
dostarczenie programistom skromnego i łatwego do zrozumienia punktu wyjściowego,
który można rozszerzać lub wykorzystywać jako inspiracje do tworzenia własnych
rodzajów treści, kontrolerów i paneli administracyjnych.

.. _`multilanguage support in PHPCR-ODM`: http://docs.doctrine-project.org/projects/doctrine-phpcr-odm/en/latest/reference/multilang.html
.. _SonataDoctrinePHPCRAdminBundle: https://github.com/sonata-project/SonataDoctrinePhpcrAdminBundle
