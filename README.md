#HipChat Alerts Module
This is a module for Drupal 7.x that uses [watchdog](https://api.drupal.org/api/drupal/includes%21bootstrap.inc/function/watchdog/7) to send important information to a HipChat room. Currently only works with [HipChat API v1](https://www.hipchat.com/docs/api) with plans to support v2 soon. For developers who maintain many different Drupal sites, this provides an easy way to monitor multiple installations and mitigate response time to resolving critical errors. Pending contrib approval on D.o. Inspired by the ChatOps movement.

##Requirements
- [Libraries 2.x](https://www.drupal.org/project/libraries)
- [hipchat-php](https://github.com/hipchat/hipchat-php)

##Features

- [x] Connect to HipChat with API v1
- [ ] Connect to HipChat with API v2
- [x] Admin config for site environments (i.e. no messages unless prod)
- [x] hook_watchdog for error reporting
- [x] Admin config for enabling different alerts 
- [ ] Build optional functionality to incorporate other contrib modules