.. highlight:: php
   :linenothreshold: 2

.. index::
    single: zdarzenia; DoctrinePHPCRBundle

Zdarzenia Doctrine PHPCR-ODM
============================

Doctrine PHPCR-ODM dostarcza system zdarzeń pozwalający reagować na wszystkie ważne
operacje, którym poodawane są dokumenty w czasie swojego cyklu życia. W celu poznania pełnego
wykazu obsługiwanych zdarzeń proszę zapoznać się z `dokumentacją systemu zdarzeń Doctrine PHPCR-ODM`_.

DoctrinePHPCRBundle dostarcza obsługę wstrzykiwania zależności dla detektorów zdarzeń
i abonentów zdarzeń.

Tagi wstrzykiwania zależności
-----------------------------

W celu nasłuchiwania zdarzeń Doctrine PHPCR-ODM można oflagować usługi. Działa to
w ten sam sposób jak dla `zdarzeń Doctrine ORM`_. Jedyne różnice, to:

* stosowanie nazwy tagu ``doctrine_phpcr.event_listener`` w postaci
``doctrine_phpcr.event_subscriber`` zamiast ``doctrine.event_listener``;
* oczekiwanie argumentu będącego klasą
  ``Doctrine\Common\Persistence\Event\LifecycleEventArgs``.

W celu oflagowania usługi jako detektora zdarzenia a innej jako abonenta zdarzenia
trzeba zastosować taka konfigurację:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        services:
            acme_search.listener.search:
                class: Acme\SearchBundle\EventListener\SearchIndexer
                    tags:
                        - { name: doctrine_phpcr.event_listener, event: postPersist }

            acme_search.subscriber.fancy:
                class: Acme\SearchBundle\EventSubscriber\MySubscriber
                    tags:
                        - { name: doctrine_phpcr.event_subscriber }

    .. code-block:: xml
       :linenos:

        <!-- src/Acme/SearchBundle/Resources/config/services.xml -->
        <?xml version="1.0" ?>
        <container xmlns="http://symfony.com/schema/dic/services">
            <services>
                <service id="acme_search.listener.search"
                         class="Acme\SearchBundle\EventListener\SearchIndexer">
                    <tag name="doctrine_phpcr.event_listener" event="postPersist" />
                </service>
                <service id="acme_search.subscriber.fancy"
                         class="Acme\SearchBundle\EventSubscriber\MySubscriber">
                    <tag name="doctrine_phpcr.event_subscriber" />
                </service>
            </services>
        </container>

    .. code-block:: php
       :linenos:

        $container
            ->register(
                'acme_search.listener.search',
                'Acme\SearchBundle\EventListener\SearchIndexer'
            )
            ->addTag('doctrine_phpcr.event_listener', array(
                'event' => 'postPersist',
            ))
        ;

        $container
            ->register(
                'acme_search.subscriber.fancy',
                'Acme\SearchBundle\EventSubscriber\FancySubscriber'
            )
            ->addTag('doctrine_phpcr.event_subscriber', array(
                'event' => 'postPersist',
            ))
        ;

.. tip::

    Abonenci zdarzeń Doctrine (zarówno ORM jak i PHPCR-ODM) mogą nie zwracać
    elastycznej tablicy metod do wywołania, tak jak mogą to czynić `abonenci
    zdarzeń Symfony`_. Abonenci zdarzeń Doctrine muszą zwracać prostą tablicę
    nazw zdarzeń swojej subskrypcji. Doctrine będzie oczekiwać metod abonenta
    z nazwami subskrybowanych zdarzeń, jak to ma miejsce podczas używania
    detektora zdarzenia.

Więcej informacji i przykładów o systemie zdarzeń Doctrine można znaleźć w artykule
"`How to Register Event Listeners and Subscribers`_" dokumentacji rdzenia.

.. _`dokumentacją systemu zdarzeń Doctrine PHPCR-ODM`: http://docs.doctrine-project.org/projects/doctrine-phpcr-odm/en/latest/reference/events.html
.. _`abonenci zdarzeń Symfony`: http://symfony.com/doc/master/components/event_dispatcher/introduction.html#using-event-subscribers
.. _`zdarzeń Doctrine ORM`: http://symfony.com/doc/current/cookbook/doctrine/event_listeners_subscribers.html
.. _`How to Register Event Listeners and Subscribers`: http://symfony.com/doc/current/cookbook/doctrine/event_listeners_subscribers.html
