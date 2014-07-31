.. index::
    single: menu; pierwsze kroki
    single: CmfMenuBundle

Strukturyzacja treści
=====================

Pakiet Menu
-----------

Koncepcja
~~~~~~~~~

Żaden system CMS nie jest kompletny bez systemu menu, który pozwala użytkownikom
poruszać się pomiędzy stronami treści i wykonywać określone działania. Choć zazwyczaj
odwzorowuje on strukturę drzewa treści, menu często posiadają własną logikę, obejmującą
opcje nie odzwierciedlone w treści lub istniejące w wielu kontekstach z wieloma opcjami,
co czyni skomplikowany problem sam w sobie.

System menu Symfony CMF
~~~~~~~~~~~~~~~~~~~~~~~

Symfony CMF SE zawiera pakiet MenuBundle, narzędzie, które pozwala na dynamiczne
definiowanie menu. Jest to rozszerzenie KnpMenuBundle_, z ustawioną hierarchicznymi,
wielojęzycznymi elementami menu, wraz z narzędziami do utrwalania ich w wybranym
magazynie danych. Zawiera również definicję panelu administracyjnego i związane
usługi potrzebne do integracji z pakietem SonataDoctrinePHPCRAdminBundle_.

.. note::

    MenuBundle rozszerza i w dużym stopniu opiera się na KnpMenuBundle_, tak więc
    należy dokładnie zapoznać się z `dokumentacja KnpMenuBundle`_. W dalszej części
    tego artykułu zakładamy, że znasz takie pojęcia jak dostawcy menu i wytwórnie menu.

Stosowanie
..........

MenuBundle wykorzystuje do wyprowadzania menu domyślne dla KnpMenuBundle renderery
i helpery. W celu poznania szczegółów proszę zapoznać się z `dokumentacją tego pakietu`_.
Podstawowe wywołanie to:

.. configuration-block::

    .. code-block:: jinja

        {{ knp_menu_render('simple') }}

    .. code-block:: php

        <?php echo $view['knp_menu']->render('simple') ?>

Dostarczona nazwa menu zostanie przekazana do implementacji ``MenuProviderInterface``,
gdzie będzie użyta do zidentyfikowania, które menu trzeba zrenderować w określonej
sekcji.

Dostawca
........

Rdzeniem MenuBundle jest ``PhpcrMenuProvider``, implementacja ``MenuProviderInterface``,
która odpowiedzialna jest za dynamiczne ładowanie menu z bazy danych PHPCR. Domyślna
usługa dostawcy jest konfigurowana z ``menu_basepath``, aby  wiedzieć gdzie ma być
odnalezione menu w drzewie PHPCR. Parametr ``name`` menu jest podawana podczas
renderowania menu i musi być bezpośrednim elementem podrzędnym bazowej ścieżki menu.
Umożliwia to aby  ``PhpcrMenuProvider`` obsługiwał kilka hierarchii menu używając
jednego mechanizmu przechowywania.

Dajmy konkretny przykład, jeśli mamy konfigurację, taka jak poniżej i renderowne
jest menu ``simple``, to główny węzeł menu musi być przechowywany w ``/cms/menu/simple``.

.. configuration-block::

    .. code-block:: yaml

        cmf_menu:
            menu_basepath: /cms/menu

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>

        <container
            xmlns="http://symfony.com/schema/dic/services"
            xmlns:cmf-menu="http://cmf.symfony.com/schema/dic/menu">

            <cmf-menu:config menu-basepath="/cms/menu"/>
        </container>

    .. code-block:: php

        $container->loadFromExtension('cmf_menu', array(
            'menu_basepath' => '/cms/menu',
        ));

Jeśli zachodzi potrzeba posiadania wielu korzeni menu, można utworzyć następne
instancje ``PhpcrMenuProvider`` i zarejestrować je w KnpMenu – w celu poznania
szczegółów zobacz kod ``DependencyInjection`` CMF MenuBundle.

Element menu pobierany za pomocą tego procesu jest używany jako główny węzeł menu
i jego węzły podrzędne są ładowane w miarę jak renderowana jest przez ``MenuFactory``
cała struktura menu.

Wytwórnia
.........

``ContentAwareFactory`` jest implementacja ``FactoryInterface`` z generacją pełnej
hierarchii ``MenuItem`` z dostarczanego MenuNode. Generowane w ten sposób dane są
faktycznie reprezentacja HTML menu.

Załączona implementacja skupia się na generowaniu instancji ``MenuItem`` z instancji
``NodeInterface``, jako że jest to zwykle najlepsze rozwiązanie do obsługi struktur
drzewiastych, zwykle wykorzystywanych przez CMS. Inne podejścia są zastosowane w
klasach bazowych. Odnośną dokumentację można znaleźć na stronie 
KnpMenuBundle_.

``ContentAwareFactory`` jest odpowiedzialny za ładowanie pełnej hierarchii
i przekształcenie instancji ``MenuNode`` z węzła główne, którą otrzymuje z implementacji
``MenuProviderInterface``. Jest również odpowiedzialny za ustalenie, czy (jeśli w ogóle)
elemnt menu jest obecnie wyświetlany przez użytkownika.
Obsługuje on mechanizm wyboru, będący kodem decydującym jaki element menu jest
bieżącym elementem.

``KnpMenu`` zawiera już określoną wytwórnię ukierunkowana na komponent Routing
Symfony2, który to pakiet rozszerza, w celu dodania obsługi dla:

* instancji ``Route`` przechowywanych w bazie danych (w celu poznania szczegółów patrz
  do :ref:`RouteProvider pakietu RoutingBundle <start-routing-getting-route-object>`)
* instancji ``Route`` z powiązaną treścią (więcej na ten temat w odpowiednim
  :ref:`rozdziale RoutingBundle <start-routing-linking-a-route-with-a-model-instance>`)

Jak wyjaśniono powyżej, ``ContentAwareFactory`` jest odpowiedzialny za ładowanie
wszystkich węzłów menu z dostarczonego elementu głównego. Rzeczywiście ładowane
węzły mogą być dowolnej klasy, nawet jeśli jst ona inna niż klasa węzła głównego,
ale one wszystkie muszą implementować ``NodeInterface`` w celu umożliwienia włączenia
do generowanego menu.

Węzły menu
..........

W MenuBundle załączony jest również dokument ``MenuNode``. Implementacja ta jest
nieco podobna do implementacji opisanej w dokumencie :doc:`static_content`.

``MenuNode`` implementuje wyżej omówiony ``NodeInterface`` i utrzymuje informacje
dotyczące pojedynczego elementu menu (``label`` i ``uri``), listy ``children``,
oraz kilka ``attributes`` dla węzła i jego węzłów podrzędnych,  które umożliwiają
dostosowywanie procesu renderowania. Zawiera również pole ``Route`` i dwa odniesienia
do elementu Content. Są one używane do przechowywania związanego obiektu ``Route``,
a także jeden element Content (a nie dwa, pomimo tego, ze istnieją dwa pola).
``MenuNode`` może mieć silne (zapewniona integralność) lub słabe (integralność nie
zapewniona) odniesienie do rzeczywistego elementu Content – to od programisty zależy
wybór najlepiej pasujący do scenariusza. Można znaleźć więcej informacji na ten temat
w `dokumentacji Doctrine PHPCR`_.

Obsługa interfejsu administracyjnego
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

MenuBundle zawiera również panele administracyjne i odpowiednie usługi potrzebne
do integracji z narzędziami zaplecza administracyjnego SonataDoctrinePHPCRAdminBundle_.

Dołączone panele administracyjne są automatycznie dostępne, ale muszą być jawnie
umieszczone na pulpicie zaplecza administracyjnego, jeśli chce się je używać.
W celu poznania instrukcji instalacji  SonataDoctrinePHPCRAdminBundle proszę zapoznać
się z :doc:`../cookbook/creating_a_cms/sonata-admin`.

Konfiguracja
~~~~~~~~~~~~

Pakiet ten jest konfigurowany przy użyciu zestawu parametrów, ale one wszystkie
są opcjonalne. Proszę zapoznać się ze strona :doc:`../bundles/menu/index` w celu
poznania pełnej listy opcji konfiguracyjnych i dodatkowych informacji.

.. _KnpMenuBundle: https://github.com/knplabs/KnpMenuBundle
.. _`dokumentacja KnpMenuBundle`: https://github.com/KnpLabs/KnpMenuBundle/blob/master/Resources/doc/index.md
.. _`dokumentacją tego pakietu`: https://github.com/KnpLabs/KnpMenuBundle/blob/master/Resources/doc/index.md#rendering-menus
.. _`dokumentacji Doctrine PHPCR`: http://docs.doctrine-project.org/projects/doctrine-phpcr-odm/en/latest/reference/association-mapping.html#references
.. _`KnpMenu`: https://github.com/knplabs/KnpMenu
.. _SonataDoctrinePHPCRAdminBundle: http://sonata-project.org/bundles/doctrine-phpcr-admin/master/doc/index.html
