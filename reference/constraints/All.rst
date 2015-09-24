All
===

Ograniczenie to umożliwia zastosować kolekcję ograniczeń do każdego elementu tablicy
(lub obiektu po którym można dokonywać trawersacji), jeśli sie go zastosuje dla
tej tablicy (obiektu). 


+-----------+-------------------------------------------------------------------+
| Dotyczy   | :ref:`właściwości lun metody<validation-property-target>`         |
+-----------+-------------------------------------------------------------------+
| Opcje     | - `constraints`_                                                  |
|           | - `payload`_                                                      |
+-----------+-------------------------------------------------------------------+
| Klasa     | :class:`Symfony\\Component\\Validator\\Constraints\\All`          |
+-----------+-------------------------------------------------------------------+
| Walidator | :class:`Symfony\\Component\\Validator\\Constraints\\AllValidator` |
+-----------+-------------------------------------------------------------------+

Podstawowe zastosowanie
-----------------------

Załóżmy, że mamy tablicę łańcuchów tekstowych i chcemy sprawdzić każdego wpis w tej tablicy:

.. configuration-block::

    .. code-block:: php-annotations

        // src/AppBundle/Entity/User.php
        namespace AppBundle\Entity;

        use Symfony\Component\Validator\Constraints as Assert;

        class User
        {
            /**
             * @Assert\All({
             *     @Assert\NotBlank,
             *     @Assert\Length(min = 5)
             * })
             */
             protected $favoriteColors = array();
        }

    .. code-block:: yaml

        # src/AppBundle/Resources/config/validation.yml
        AppBundle\Entity\User:
            properties:
                favoriteColors:
                    - All:
                        - NotBlank:  ~
                        - Length:
                            min: 5

    .. code-block:: xml

        <!-- src/AppBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="AppBundle\Entity\User">
                <property name="favoriteColors">
                    <constraint name="All">
                        <option name="constraints">
                            <constraint name="NotBlank" />
                            <constraint name="Length">
                                <option name="min">5</option>
                            </constraint>
                        </option>
                    </constraint>
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/AppBundle/Entity/User.php
        namespace AppBundle\Entity;

        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints as Assert;

        class User
        {
            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('favoriteColors', new Assert\All(array(
                    'constraints' => array(
                        new Assert\NotBlank(),
                        new Assert\Length(array('min' => 5)),
                    ),
                )));
            }
        }

Teraz każdy wpis w tablicy ``favoriteColors`` będzie sprawdzony pod kątem tego,
czy nie jest pusty i czy ma co najmniej 5 znaków.

Opcje
-----

constraints
~~~~~~~~~~~

**typ**: ``array`` [:ref:`default option<validation-default-option>`]

Ta obowiązkowa opcja jest tablicą ograniczeń walidacyjnych, które chce się
zastosować dla każdego elementu danej tablicy.

.. include:: /reference/constraints/_payload-option.rst.inc
