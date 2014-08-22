.. highlight:: php
   :linenothreshold: 2

.. index::
    double: wielojęzyczność; cmf

Obsługa wielojęzycznych dokumentów
==================================

Domyślna warstwa magazynowania CMF – PHPCR-ODM – może przejrzyście obsługiwać tłumaczenia.

.. tip::

   W tym rozdziale zakłada się, że masz już wiedzę o interakcji z PHPCR-ODM,
   co omówione zostało w :doc:`rozdziale o bazie danych <database_layer>`.

.. caution::

   Musisz mieć też zainstalowane i włączone rozszerzenie php ``intl``(inaczej
   Composer powiadomi, że *can't find ext-intl*). Jeśli masz problemy związane
   z niemożliwością ładowania niektórych ustawień, zajrzyj do
   `dyskusji na ten temat na ICU`_.

Początkowy wybór języka: Lunetics LocaleBundle
----------------------------------------------

CMF zaleca, aby do obsługi żądań z adresem ``/`` oprzeć się na pakiecie
`LuneticsLocaleBundle`_. Pakiet ten udostępnia narzędzia do wyboru najlepszego
jezyka dla określonego użytkownika, w oparciu o różne kryteria.

Podczas konfigurowania ``lunetics_locale`` zaleca się, aby wykorzystać także parametr
ustawień regionalnych innych pakietów (np. CoreBundle).

.. configuration-block::

    .. code-block:: yaml

        lunetics_locale:
            allowed_locales: "%locales%"

    .. code-block:: xml

        <?xml version="1.0" charset="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">

            <config xmlns="http://example.org/schema/dic/lunetics_locale">
                <allowed-locales>%locales%</allowed-locales>
            </config>
        </container>

    .. code-block:: php

        $container->loadFromExtension('lunetics_locale', array(
            'allowed_locales' => '%locales%',
        ));

Konfiguracja dostępnych ustawień regionalnych
---------------------------------------------

CoreBundle musi zostać skonfigurowany z dostępnymi językami.
Jeśli nie ma skonfigurowanych ustawień regionalnych, to rejestrowany jest nasłuch,
który usuwa wszystkie odwzorowania tłumaczeń z dokumentów PHPCR-ODM.

.. configuration-block::

    .. code-block:: yaml

        cmf_core:
            multilang:
                locales: "%locales%"

    .. code-block:: xml

        <?xml version="1.0" charset="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">

            <config xmlns="http://cmf.symfony.com/schema/dic/core">
                <multilang>%locales%</multilang>
            </config>
        </container>

    .. code-block:: php

        $container->loadFromExtension('cmf_core', array(
            'multilang' => array(
                'locales' => '%locales%',
            ),
        ));

Wielojęzyczne dokumenty PHPCR-ODM
---------------------------------

Można oznaczyć wszystkie właściwości jako podlegające tłumaczeniu i mieć magazyn
zarządzanymi dokumentami oraz ładować właściwy język. Trzeba mieć na uwadze, że
tłumaczenie zawsze odbywa się na poziomie dokumentu, a nie w indywidualnych polach
tłumaczeń.

.. code-block:: php
   :linenos:

    <?php

    // src/Acme/DemoBundle/Document/MyPersistentClass.php

    use Doctrine\ODM\PHPCR\Mapping\Annotations as PHPCR;

    /**
     * @PHPCR\Document(translator="attribute")
     */
    class MyPersistentClass
    {
        /**
         * Translated property
         * @PHPCR\String(translated=true)
         */
        private $topic;

        // ...
    }

.. seealso::
   
   Przeczytaj więcej o dokumentach wielojęzycznych w `dokumentacji PHPCR-ODM o
   wielojęzyczności`_ i zobacz :doc:`../bundles/phpcr_odm/multilang` w celu
   uzyskania informacji o prawidłowej konfiguracji PHPCR-ODM.

Domyślne dokumenty dostarczane przez pakiety CMF są dokumentami przetłumaczonymi.
CoreBundle usuwa odwzorowanie tłumaczeń, jeśli opcja ``multilang`` nie jest skonfigurowana.

Trasy są obsługiwane inaczej, o czym można się dowiedzieć w następnym rozdziale.

Trasowanie
----------

``DynamicRouter`` wykorzystuje źródło trasy do pobierania tras, które mogą być
dopasowane do żądania. Zamysłem domyślnego źródła PHPCR-ODM jest odwzorowanie
żądanego URL na identyfikator, którym w terminologii PHPCR jest ścieżką
repozytoryjną do węzła. Pozwala to na bardzo wydajne wyszukiwanie bez potrzeby
pełnego przeszukiwania repozytorium. Lecz węzeł PHPCR ma dokładnie jedną ścieżkę,
dlatego potrzeba oddzielnego dokumentu trasy dla każdego języka.
Możliwość regionalizacji lokalizatora URL dla poszczególnych języków, to dobra rzecz. 
Wystarczy utworzyć jeden dokument dla każdego języka.

Ponieważ wszystkie trasy wskazują na tą samą treść, generator trasy może je obsługiwać
wybierając właściwą trasę, gdy generuje się trasę z treści. Przeczytaj też 
":ref:`ContentAwareGenerator and Locales <component-route-generator-and-locales>`".

.. _book_handling-multilang_sonata-admin:

Sonata PHPCR-ODM Admin
----------------------

.. note::
   
   Zastosowanie zaplecza administracyjnego Sonata jest jednym ze sposobów umożliwienia
   edycji treści. Planowane jest wydanie rozdziału tego podręcznika poświęconego
   zapleczu administracyjnemu. Tymczasem prosimy zapoznać się z
   :doc:`Zaplecze administracyjne Sonata <../cookbook/creating_a_cms/sonata-admin>`
   w poradniku "Tworzenie CMS".

Pierwszym krokiem jest skonfigurowanie zaplecza administracyjnego Sonata. Można
umieścić przełącznik językowy LuneticsLocaleBundle na pasku ``topnav``. Do zrobienia
tego trzeba skonfigurować szablon dla ``user_block``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        sonata_admin:
            # ...
            templates:
                    user_block: AcmeCoreBundle:Admin:admin_topnav.html.twig

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <config xmlns="http://sonata-project.org/schema/dic/admin">
                <template user-block="AcmeCoreBundle:Admin:admin_topnav.html.twig"/>
            </config>
        </container>


    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('sonata_admin', array(
            'templates' => array(
                'user_block' => 'AcmeCoreBundle:Admin:admin_topnav.html.twig',
            ),
        ));

Szablon ten będzie wygladał podobnie do tego:

.. code-block:: jinja

    {# src/Acme/CoreBundle/Resources/views/Admin/admin_topnav.html.twig #}
    {% extends 'SonataAdminBundle:Core:user_block.html.twig' %}

    {% block user_block %}
        {{ locale_switcher(null, null, 'AcmeCoreBundle:Admin:switcher_links.html.twig') }}
        {{ parent() }}
    {% endblock %}

Trzeba powiadomić ``locale_switcher`` aby używał indywidualnego szablonu do
wyświetlania odnośników, co może wyglądać tak:

.. code-block:: jinja

    {# src/Acme/CoreBundle/Resources/views/Admin/switcher_links.html.twig #}
    Switch to :
    {% for locale in locales %}
        {% if loop.index > 1 %} | {% endif %}<a href="{{ locale.link }}" title="{{ locale.locale_target_language }}">{{ locale.locale_target_language }}</a>
    {% endfor %}

Teraz pozostało tylko zaktualizowanie tras Sonaty, aby reagowały na wskazania językowe:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/routing.yml

        admin_dashboard:
            pattern: /{_locale}/admin/
            defaults:
                _controller: FrameworkBundle:Redirect:redirect
                route: sonata_admin_dashboard
                permanent: true # this for 301

        admin:
            resource: '@SonataAdminBundle/Resources/config/routing/sonata_admin.xml'
            prefix: /{_locale}/admin

        sonata_admin:
            resource: .
            type: sonata_admin
            prefix: /{_locale}/admin

        # redirect routes for the non-locale routes
        admin_without_locale:
            pattern: /admin
            defaults:
                _controller: FrameworkBundle:Redirect:redirect
                route: sonata_admin_dashboard
                permanent: true # this for 301

        admin_dashboard_without_locale:
            pattern: /admin/dashboard
            defaults:
                _controller: FrameworkBundle:Redirect:redirect
                route: sonata_admin_dashboard
                permanent: true

    .. code-block:: xml
       :linenos:

        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/dic/routing">

            <route id="admin_dashboard" pattern="/{_locale}/admin/">
                <default key="_controller">FrameworkBundle:Redirect:redirect</default>
                <default key="route">sonata_admin_dashboard</default>
                <default "permanent">true</default>
            </route>

            <import resource="@SonataAdminBundle/Resources/config/routing/sonata_admin.xml"
                    prefix="/{_locale}/admin"
            />

            <import resource="." type="sonata_admin" prefix="/{_locale}/admin"/>

            <!-- redirect routes for the non-locale routes -->
            <route id="admin_without_locale" pattern="/admin">
                <default key="_controller">FrameworkBundle:Redirect:redirect</default>
                <default key="route">sonata_admin_dashboard</default>
                <default "permanent">true</default>
            </route>
            <route id="admin_dashboard_without_locale" pattern="/admin/dashboard">
                <default key="_controller">FrameworkBundle:Redirect:redirect</default>
                <default key="route">sonata_admin_dashboard</default>
                <default "permanent">true</default>
            </route>
        </routes>

    .. code-block:: php
       :linenos:

        // app/config/routing.php

        $collection = new RouteCollection();

        $collection->add('admin_dashboard', new Route('/{_locale}/admin/', array(
            '_controller' => 'FrameworkBundle:Redirect:redirect',
            'route' => 'sonata_admin_dashboard',
            'permanent' => true,
        )));

        $sonata = $loader->import('@SonataAdminBundle/Resources/config/routing/sonata_admin.xml');
        $sonata->addPrefix('/{_locale}/admin');
        $collection->addCollection($sonata);

        $sonata = $loader->import('.', 'sonata_admin');
        $sonata->addPrefix('/{_locale}/admin');
        $collection->addCollection($sonata);

        $collection->add('admin_without_locale', new Route('/admin', array(
            '_controller' => 'FrameworkBundle:Redirect:redirect',
            'route' => 'sonata_admin_dashboard',
            'permanent' => true,
        )));

        $collection->add('admin_dashboard_without_locale', new Route('/admin/dashboard', array(
            '_controller' => 'FrameworkBundle:Redirect:redirect',
            'route' => 'sonata_admin_dashboard',
            'permanent' => true,
        )));

        return $collection

Teraz, trzeba przeładować pulpit administracyjny. Po tym zabiegu URL powinien
być poprzedzony symbolem domyślnego języka, na przykład ``/pl/admin/dashboard``.
Po kliknięciu na przełącznik językowy, strona zostanie przeładowana i wyświetlona
będzie treść właściwa dla żądanego języka.

Gdy dokumenty implentuja TranslatableInterface można 
:ref:`skonfigurować rozszerzenie translacyjne zaplecza admnistracyjnego
<bundle-core-translatable-admin-extension>`,
aby pobierać pole wybranego języka i pozwalać administratorowi wybierać, który
język ma być przechowywany w treści.

Edycja frontonowa a wielojęzyczność
-----------------------------------

Podczas używania CreateBundle nie musi się nic robić, aby pobierać wsparcie
wielojęzyczności. PHPCR-ODM dostarcza dokument w żądanym języku i zostanie
wygenerowany w żądanym języku lokalizator URL wywołania zwrotnego, prowadząc do 
zapisu edytowanego dokumentu w tym samym języku, w którym został załadowany.

.. note::

   Jeśli brakuje tłumaczenia, następuje awaryjny zrzut językowy, zarówno podczas
   wyświetlania strony, ale również podczas zapisywania zmian. Tak więc zawsze
   należy aktualizować bieżące ustawienie językowe.
   
   Wydaje się rozsądne, zaoferowanie użytkownikowi wyboru, czy chce utworzyć nowe
   tłumaczenie, czy zaktualizować istniejące. Jest to `problem`_ opisany w systemie
   śledzenia spraw CreateBundle.

.. _`LuneticsLocaleBundle`: https://github.com/lunetics/LocaleBundle/
.. _`dyskusji na ten temat na ICU`: https://github.com/symfony/symfony/issues/5279#issuecomment-11710480
.. _`cmf-sandbox config.yml file`: https://github.com/symfony-cmf/cmf-sandbox/blob/master/app/config/config.yml
.. _`PHPCR-ODM documentation on multi-language`: http://docs.doctrine-project.org/projects/doctrine-phpcr-odm/en/latest/reference/multilang.html
.. _`problem`: https://github.com/symfony-cmf/CreateBundle/issues/39
