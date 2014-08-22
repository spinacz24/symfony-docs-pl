.. highlight:: php
   :linenothreshold: 2

.. index::
    single: RoutingAuto; pakiety
    single: RoutingAutoBundle

RoutingAutoBundle
=================

    Pakiet RoutingAutoBundle pozwala automatycznie definiować trasy dla dokumentów.

.. include:: ../_not-stable-caution.rst.inc

Oznacza oddzielenie dokumentów ``Route`` i ``Content``. Jeśli potrzeby są małe,
to ten pakiet może się nie nadawać i należy zapoznać się z
:doc:`dokumentacją SimpleCmsBundle <../simple_cms/introduction>`.

Instalacja
----------

Można zainstalować pakiet `poprzez Composer`_ używając pakietu
`symfony-cmf/routing-auto-bundle`_.

Możliwości
----------

Proszę sobie wyobrazić, że mamy zamiar utworzyć aplikację forum, która ma dwa
trasowane dokumenty treści: kategorię i tematy. Dokumenty maja nazwy ``Category``
i ``Topic`` i są nazywane *dokumentami treści*.

Jeśli utworzymy nową kategorię z tytułem "My New Category", RoutingAutoBundle
będzie automatycznie tworzyć trasę ``/forum/my-new-cateogry``. Dla każdej nowej
kategorii  ``Topic`` będzie to tworzyć trasę podobna do tej
``/forum/my-new-category/my-new-topic``. Ta ścieżka URL rozwiązuje specjalny typ
trasy, który nazywa się *automatyczną trasą*.

Domyślnie, podczas aktualizowania dokumentu treści, który ma automatyczne trasowanie,
zostanie zaktualizowana odpowiadająca mu automatyczna trasa. Podczas usuwania dokumentu
treści, usuwana jest odpowiadająca mu trasa automatyczna.

Jeśli jest to wymagane, pakiet można również skonfigurować do robienia dodatkowych
rzeczy, jak na przykład, opuszczenia ``RedirectRoute`` podczas zmiany położenia
dokumentu treści lub automatycznego wyświetlania strony indeksowej, gdy dostępna
jest nieskonfigurowana ścieżka pośrednia (np. listująca wszystkie dokumenty podrzędne
przy żądaniu ``/forum``, zamiast zwracania strony ``404``).

Dlaczego po prostu nie użyć pojedynczej trasy?
----------------------------------------------

Oczywiście nasza fikcyjna aplikacja forum może używać pojedynczej trasy w wzorcem
``/forum/my-new-forum/{topic}``, która może być obsługiwana przez kontroler.
Dlaczego po prostu tego nie zrobić?

#. Mając trasę dla każdej strony w systemie, aplikacja ma wiedzę o dostępnych
   ścieżkach URL. Może to być bardzo pomocne, na przykład, podczas określania
   punktów końcowych dla elementów menu, które są używane podczas generowania
   mapy witryny;
#. Oddzielając trasę od treści, umożliwia się, aby trasa mogla być dostosowywana
   niezależnie od treści, na przykład, temat może mieć ten sam tytuł jak inny temat,
   ale wymaga innego URL;
#. Oddzielne dokumenty tras są możliwe do tłumaczenia – co oznacza, że dla
   *każdego języka* można mieć odrębna ścieżkę URL, na przykład,"/welcome"
   i "/powitanie" by każda z nich przywoływała ten sam dokument, odpowiednio
   w języku angielskim i polskim. Byłoby to trudne, gdyby alias (*ang. slug*)
   został osadzony w dokumencie treści;
#. Dzięki oddzieleniu trasy od treści aplikację nie obchodzi *do czego* odwołuje
   się dokument trasy. Oznacza to, że można łatwo wymienić klasę danego dokumentu.

Stosowanie
----------

Poniższy diagram pokazuje fikcyjna ścieżkę URL dla tematu forum. Pierwsze 6 elementów
tej ścieżki nosi nazwę *ścieżki treści (ang. content path )*. Następny element nazywa
się *nazwą treści (ang. content name)*.

.. image:: ../../_images/bundles/routing_auto_post_schema.png

Ścieżka treści jest podzielona na *jednostki ścieżki (ang. path units)* i *elementy
ścieżki (ang. path elements)*. Jednostka ścieżki jest grupą elementów ścieżki
a elementy ścieżki są po prostu dokumentami w drzewie PHPCR.

.. note::

    Chociaż w tym przypadku elementy ścieżki mogą być dowolna klasą dokumentu,
    to tylko obiekty rozszerzające klasę :class:`Symfony\\Component\\Routing\\Route`
    będą brane pod uwagę w czasie dopasowywania ścieżki URL.

    Domyślnym zachowaniem podczas generowania ścieżki treści  jest użycie dokumentu
    ``Generic``. Dokumenty te będą powodować błąd 404 podczas bezpośredniego dostępu.

Wewnętrznie każda jednostka ścieżki jest budowana przez *budowniczego jednostek*.
Budowniczy jednostek zawiera jedną klasę *dostawcy ścieżekr* i dwie akcje – jedna
akcja podejmowana jest gdy dostarczona ścieżka istnieje w drzewie PHPCR, a druga
w przeciwnym przypadku. Celem każdego budowniczego jednostek jest generowanie ścieżki
i dostarczenie następnie obiektu trasy  dla każdego elementu tej ścieżki.

Konfiguracja dla powyższego przykładu może wyglądać tak:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        cmf_routing_auto:
            mappings:

                Acme\ForumBundle\Document\Topic
                    content_path:
                        # corresponds first path unit in diagram: my-forum
                        forum_path:
                            provider: [specified, { path: my-form }]
                            exists_action: use
                            not_exists_action: create

                        # corresponds second path unit in diagram: my-category
                        category_path:
                            provider: [content_object, { method: getCategory }]
                            exists_action: use
                            not_exists_action: throw_exception

                    # corresponds to the content name: my-new-topic
                    content_name:
                        provider: [content_method, { method: getTitle }]
                        exists_action: [auto_increment, { pattern: -%d }]
                        not_exists_action: create

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">

            <config xmlns="http://cmf.symfony.com/schema/dic/routing_auto">

                <mapping class="Acme\ForumBundle\Document\Topic">

                    <content-path>
                        <!-- corresponds first path unit in diagram: my-forum -->
                        <path-unit name="forum_path">
                            <provider name="specified">
                                <option name="path" value="my-forum" />
                            </provider>
                        </path-unit>

                        <!-- corresponds second path unit in diagram: my-category -->
                        <path-unit name="category_path">
                            <provider name="content_object">
                                <option name="method" value="getCategory" />
                            </provider>
                            <exists-action strategy="use" />
                            <not-exists-action strategy="throw_exception" />
                        </path-unit>
                    </content-path>


                    <!-- corresponds to the content name: my-new-topic -->
                    <content-name>
                        <provider name="content_method">
                            <option name="method" value="getTitle" />
                        </provider>
                        <exists-action strategy="auto_increment">
                            <option name="pattern" value="-%d" />
                        </exists-action>
                        <not-exists-action strategy="create" />
                    </content-name>
                </mapping>
            </config>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('cmf_routing_auto', array(
            'mappings' => array(
                'Acme\ForumBundle\Document\Topic' => array(
                    'content_path' => array(
                        // corresponds first path unit in diagram: my-forum
                        'forum_path' => array(
                            'provider' => array('specified', array(
                                'path' => 'my-forum',
                            )),
                            'exists_action' => 'use',
                            'not_exists_action' => 'create',
                        ),

                        // corresponds second path unit in diagram: my-category
                        'category_path' => array(
                            'provider' => array('content_object', array(
                                'method' => 'getCategory',
                            )),
                            'exists_action' => 'use',
                            'not_exists_action' => 'throw_exception',
                        ),
                    ),

                    // corresponds to the content name: my-new-topic
                    'content_name' => array(
                        'provider' => array('content_method', array(
                            'method' => 'getTitle',
                        )),
                        'exists_action' => array('auto_increment', array(
                            'pattern' => '-%d',
                        )),
                        'not_exists_action' => 'create',
                    ),
                ),
            ),
        ));

Następnie trzeba będzie utworzyć dokument ``Topic`` implementujący wyżej wymienionych
metody, w ten sposób::

    // src/Acme/ForumBundle/Document/Topic.php
    namespace Acme\ForumBundle\Document;

    class Topic
    {
        /**
         * Returns the category object associated with the topic.
         */
        public function getCategory()
        {
            return $this->category;
        }

        public function getPublishedDate()
        {
            return new \DateTime('2013/04/06');
        }

        public function getTitle()
        {
            return "My Topic Title";
        }
    }

Po utrwaleniu tego obiektu zostanie utworzona trasa. Oczywiście trzeba wykonać
edytowalne właściwości, co w konsekwencji da w pełni sprawny system trasowania.

.. note::

    Wszystkie odwzorowania do obiektu będą miały również zastosowanie do podklas
    tego obiektu. Wyobraź sobie, ze mamy 2 dokumenty, ``ContactPage`` i ``Page``,
    które obydwa rozszerzają klase ``AbstractPage``. Podczas odwzorowania  klasy
    ``AbstractPage``, będzie ono również skutkowało na te dwa dokumenty.

Dostarczani dostawcy i akcje
----------------------------
Pakiet RoutingAutoBundle dostarczany jest wraz z kilkoma dostawcami ścieżek
i akcjami, które są domyślne.
Przeczytaj więcej na ten temat w rozdziałach:

* :doc:`providers`
* :doc:`exists_actions`
* :doc:`not_exists_actions`

Dostosowanie
------------

Oprócz domyślnych dostawców i akcji, można również tworzyć własne. Przeczytaj na
ten temat w :doc:`customization`.

.. _`poprzez Composer`: http://getcomposer.org/
.. _`symfony-cmf/routing-auto-bundle`: https:/packagist.org/packages/symfony-cmf/routing-auto-bundle
