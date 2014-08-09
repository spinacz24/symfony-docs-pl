.. index::
    single: typy formularzy; DoctrinePHPCRBundle

Typy formularzy PHPCR-ODM
=========================

Pakiet ten zawiera kilka typów formularzy dla PHPCR i PHPCR-ODM przydatnych
w określonych przypadkach, wraz z odgadywaczem typów, który wykorzystuje te typy.

Istnieje również ograniczony walidator dla dokumentów PHPCR-ODM.

Typy formularzy
---------------

.. tip::

    Podczas edytowania skojarzeniowych pól wielowartościowych miej wzgląd
    na BurgovKeyValueFormBundle_.

phpcr_document
~~~~~~~~~~~~~~

Ten typ formularza jest odpowiedni do edycji powiązanych dokumentów PHPCR-ODM.
Działa on dla ReferenceOne, ReferenceMany i Referrers, ale także dla skojarzonych 
obiektów ParentDocument. Trzeba się upewnić, że opcja ``multiple`` ustawiona jest
dla ReferenceMany i Referrers, a nie ustawiona dla pozostałych obiektów.

.. note::

    Ponieważ ``Children`` jest też skojarzony, to nie ma sensu edytowania go w tym
    typie formularza. Obiekty potomne są automatycznie dołączane do swoich elementów
    nadrzędnych. ``MixedReferrers`` może być pokazywany jako pole ``disabled``,
    ale nigdy nie może być edytowany, ponieważ skojarzenie jest niezmienne.

Ten typ formularza jest równoważny z typem formularza ``entity`` dostarczanym
przez Symfony dla Doctrine ORM. Ma on te same opcje jak typ ``entity``, z tym że
opcja dla menadżera dokumentu ma nazwę ``em``.

Prosty przykład wykorzystania typu formularza ``phpcr_document`` wygląda następująco::

    $form
        ->add(
            'speakers',
            'phpcr_document',
            array(
                'property' => 'title',
                'class'    => 'Acme\DemoBundle\Document\TargetClass',
                'multiple' => true,
            )
        )
    ;

Wyprodukuje to pole wielokrotnego wyboru z wartością ``getTitle`` wywoływana
w każdej instancji ``TargetClass`` znalezionej w repozytorium treści. Alternatywnie,
można ustawić opcję ``choices`` do listowania dostępnych do zarządzania dokumentów.
W celu poznania szczegółów, proszę zapoznać się z `Dokumentacją Symfony dla encji
typów formularzy`_, w tym w jaki sposób można skonfigurować zapytania.

Jeśli używa się SonataDoctrinePHPCRAdminBundle_, warto przyjrzeć się ``sonata_type_collection``.
Ten typ formularza pozwala edytować wierszowo powiązane dokumenty (referencje a także
dokumenty podrzędne) oraz tworzyć i usuwać je w locie.

phpcr_odm_reference_collection
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. caution::

    Ten typ formularza został zdeprecjonowany w DoctrinePHPCRBundle 1.1 i będzie
    całkowicie usunięty w DoctrinePHPCRBundle 1.2. Zamiast niego należy użyć typ
    `phpcr_document`_, który może robić to samo ale lepiej.

Ten typ formularza obsługuje edycję kolekcji ``ReferenceMany`` na dokumentach
PHPCR-ODM. Jest to pole wyboru z dodatkiem ``referenced_class`` wymagająca opcji,
która określa  klasę przywoływanego dokumentu docelowego.

Używanie tego typ formularza wymaga również określenie listy możliwych celów
referencyjnych w postaci tablicy identyfikatorów PHPCR-ODM lub ścieżek PHPCR.

Minimalny kod wymagany do użycia tego typu wygląda następująco::

    $dataArr = array(
        '/some/phpcr/path/item_1' => 'first item',
        '/some/phpcr/path/item_2' => 'second item',
    );

    $formMapper
        ->with('form.group_general')
            ->add('myCollection', 'phpcr_odm_reference_collection', array(
                'choices'   => $dataArr,
                'referenced_class'  => 'Class\Of\My\Referenced\Documents',
            ))
        ->end();

.. tip::

    Podczas budowania interfejsu administracyjnego w SonataDoctrinePHPCRAdminBundle_
    istnieje jeszcze ``sonata_type_model``, który jest jeszcze bardziej użyteczny,
    umożliwiając dodawanie powiązanych dokumentów w locie. Niestety jest on `obecnie
    uszkodzony`_.

phpcr_reference
~~~~~~~~~~~~~~~

Typ ``phpcr_reference`` reprezentuje w formularzu właściwość PHPCR typu REFERENCE
lub WEAKREFERENCE. Pole wejściowe berdzie renderowane jako pole tekstowe zawierające
PATH albo UUID w zależności od konfiguracji. Formularz, przy ustawianiu odwołania
rozpoznaje ścieżkę lub identyfikator.

Typ ten rozszerza typ formularza ``text``. Dodaje opcję ``transformer_type``, która
może być ustawiona jako ``path`` albo ``uuid``.


Ograniczenia walidatora
-----------------------

Pakiet dostarcza ograniczenie walidatora ``ValidPhpcrOdm``, które można wykorzystać
do sprawdzenia czy  dokument ``Id`` lub ``Nodename`` i pole ``Parent`` są właściwe.

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/BlogBundle/Resources/config/validation.yml
        Acme\BlogBundle\Entity\Author:
            constraints:
                - Doctrine\Bundle\PHPCRBundle\Validator\Constraints\ValidPhpcrOdm

    .. code-block:: php-annotations

        // src/Acme/BlogBundle/Entity/Author.php

        // ...
        use Doctrine\Bundle\PHPCRBundle\Validator\Constraints as OdmAssert;

        /**
         * @OdmAssert\ValidPhpcrOdm
         */
        class Author
        {
            // ...
        }

    .. code-block:: xml

        <!-- Resources/config/validation.xml -->
        <?xml version="1.0" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping
                http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="Symfony\Cmf\Bundle\RoutingBundle\Doctrine\Phpcr\Route">
                <constraint name="Doctrine\Bundle\PHPCRBundle\Validator\Constraints\ValidPhpcrOdm" />
            </class>

        </constraint-mapping>

    .. code-block:: php

        // src/Acme/BlogBundle/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Doctrine\Bundle\PHPCRBundle\Validator\Constraints as OdmAssert;

        /**
         * @OdmAssert\ValidPhpcrOdm
         */
        class Author
        {
            // ...

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addConstraint(new OdmAssert\ValidPhpcrOdm());
            }
        }

.. _BurgovKeyValueFormBundle: https://github.com/Burgov/KeyValueFormBundle
.. _`Symfony documentation on the entity form type`: http://symfony.com/doc/current/reference/forms/types/entity.html
.. _SonataDoctrinePHPCRAdminBundle: http://sonata-project.org/bundles/doctrine-phpcr-admin/master/doc/index.html
.. _`obecnie uszkodzony`: https://github.com/sonata-project/SonataDoctrineORMAdminBundle/issues/145
