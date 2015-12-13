.. note::

    Ze wzgledu na to, w jaki sposób parametry sa rozwiązywane, nie można ich
    używać do budowy ścieżek w dynamicznym importowaniu. Oznacza to, że coś
    takiego nie będzie działać:

    .. configuration-block::

        .. code-block:: yaml

            # app/config/config.yml
            imports:
                - { resource: "%kernel.root_dir%/parameters.yml" }

        .. code-block:: xml

            <!-- app/config/config.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    http://symfony.com/schema/dic/services/services-1.0.xsd">

                <imports>
                    <import resource="%kernel.root_dir%/parameters.yml" />
                </imports>
            </container>

        .. code-block:: php

            // app/config/config.php
            $loader->import('%kernel.root_dir%/parameters.yml');
