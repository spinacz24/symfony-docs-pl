.. highlight:: php
   :linenothreshold: 2

.. index::
    single: rozszerzanie klasy Page; SimpleCmsBundle

Rozszerzani klasy Page
----------------------

Domyślny dokument Page (``Symfony\Cmf\Bundle\SimpleCmsBundle\Model\Page``)
jest stosunkowo prosty, dostarczany wraz z paru najbardziej użytecznymi właściwościami
dla budowania typowej strony: tytułem, treścią, znacznikami, datą publikacji itd.

Jeśli to nie wystarcza w projekcie, można łatwo dostarczyć własny dokument przez
rozszerzenie  domyślnego dokumentu ``Page`` i jawne ustawienie parametru konfiguracji
dla własnej klasy dokumentu:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        cmf_simple_cms:
            persistence:
                phpcr:
                    document_class: Acme\DemoBundle\Document\MySuperPage

    .. code-block:: xml
       :linenos:

        <?xml version="1.0" charset="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">

            <config xmlns="http://cmf.symfony.com/schema/dic/simplecms">
                <persistence>
                    <phpcr
                        document-class="Acme\DemoBundle\Document\MySuperPage"
                    />
                </persistence>
            </config>

        </container>

    .. code-block:: php
       :linenos:

        $container->loadFromExtension('cmf_simple_cms', array(
            'persistence' => array(
                'phpcr' => array(
                    'document_class' => 'Acme\DemoBundle\Document\MySuperPage',
                ),
            ),
        ));

Alternatywnie, domyślny dokument ``Page`` zawiera właściwość ``extras``.
Przechowuje ona tablicę elementów klucz – wartość (gdzie klucz musi przechowywać
jakąś wartość albo ``null``), która można wykorzystać dla małych, trywialnych
dodatków, bez konieczności rozszerzania dokumentu Page.

Na przykład::

    use Symfony\Cmf\Bundle\SimpleCmsBundle\Doctrine\Phpcr\Page;
    // ...

    $page = new Page();

    $page->setTitle('Hello World!');
    $page->setBody('Really interesting stuff...');
    $page->setLabel('Hello World');

    // set extras
    $extras = array(
        'subtext' => 'Add CMS functionality to applications built with the Symfony2 PHP framework.',
        'headline-icon' => 'exclamation.png',
    );

    $page->setExtras($extras);

    $documentManager->persist($page);

Właściwości te mogą być następnie dostępne w kontrolerze lub szablonie poprzez
metody ``getExtras()`` lub ``getExtra($key)``.
