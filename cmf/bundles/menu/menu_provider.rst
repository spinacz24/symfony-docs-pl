.. index::
    single: wytwórnia menu; MenuBundle
    single: dostawca menu; MenuBundle

Dostawca menu
=============

Dostawca menu jest odpowiedzialny za ładowanie menu, gdy jest to wymagane.
KnpMenu obsługuje kilku dostawców. Pakiet MenuBundle udostępnia ``PhpcrMenuProvider``
do ładowania elementów menu z PHPCR-ODM.

Każde menu jest identyfikowane przez nazwę swojego węzła głównego, na ścieżce
określonej przez klucz konfiguracyjny ``persistence.phpcr.menu_basepath``.
Domyślną ścieżką jest ``/cms/menu``, więc w poniższym przykładzie struktury
dokumentu ``main-menu`` i ``side-menu`` są obie prawidłowymi nazwami menu:

.. code-block:: text

    ROOT
        cms:
            menu:
                main-menu:
                    item-1:
                    item-2:
                side-menu:
                    item-1:

W razie potrzeby można użyć własnych klas dokumentów dla węzłów menu, jeśli
implementują ``Knp\Menu\NodeInterface`` (tak aby integrowały się z KnpMenuBundle).
Domyślna klas ``MenuNode`` odrzuca elementy potomne, które nie implementują
``Knp\Menu\NodeInterface``.

.. note::

    Nie ma jeszcze wsparcia dla Doctrine ORM lub innych menadżerów utrwalania.
    Niestety, jeszcze nikt tego nie zbudował. Będziemy wdzięczni za zgłoszenie
    aktualizacji (*ang. pull request*) z refaktoryzacją ``PhpcrMenuProvider``
    do odpowiedniej klasy bazowej dla wszystkich implementacji Doctrine
    i przechowywaniem określonych dostawców.

Można również napisać od nowa zupełnie indywidulanego dostawcę i zarejestrować
go w KnpMenu, taka jak wyjaśniono to w
:doc:`/knp_bundles/KnpMenuBundle/custom_provider`.


