<?php

/**
 * @file
 * Provides internal functions for Facebook Tab Applications.
 */


/**
 * Returns Facebook Tab Application Object.
 *
 * @return
 *   An object of Facebook Application settings.
 */
function fbtab_app() {
  $app = (object) array(
    'id' => variable_get('fbtab_app_id', ''),
    'secret' => variable_get('fbtab_app_secret', ''),
    'auth_redirect' => variable_get('fbtab_app_auth_redirect', ''),
    'link_accounts' => variable_get('fbtab_app_link_accounts', FALSE),
    'link_accounts_pass_bypass' => variable_get('fbtab_app_link_accounts_pass_bypass', FALSE),
    'request' => fbtab_signed_request(),
  );

  return $app;
}

/**
 * Returns Facebook Signed Request.
 *
 * @return
 *   An array of Facebook settings or FALSE.
 */
function fbtab_signed_request() {
  $signed_request = '';

  // Get signed request from $_REQUEST.
  if ( isset($_REQUEST['signed_request']) ) {
    $signed_request = $_REQUEST['signed_request'];
    $_SESSION['fbtab_signed_request'] = $_REQUEST['signed_request'];
  }
  // Get signed request from $_SESSION.
  elseif ( isset($_SESSION['fbtab_signed_request']) ) {
    $signed_request = $_SESSION['fbtab_signed_request'];
  }

  // Parse signed request.
  if ( !empty($signed_request) ) {
    list($encoded_sig, $payload) = explode('.', $signed_request, 2);
    $request = json_decode(base64_decode(strtr($payload, '-_', '+/')), true);
  }
  else {
    $request = FALSE;
  }

  return $request;
}

/**
 * Forces user to authorize application.
 */
function fbtab_authorize($redirect_uri = NULL) {
  global $fbtab_app;

  // Default redirect to fbtab_auth_redirect
  if ( is_null($redirect_uri) ) {
    $redirect_uri = $fbtab_app->auth_redirect;
  }

  // Application hasn't been authorized, redirect to authorization page.
  if ( $fbtab_app->request && empty($fbtab_app->request['user_id']) ) {
    $auth_url  = 'https://www.facebook.com/dialog/oauth?';
    $auth_url .= 'client_id=' . $fbtab_app->id . '&';
    $auth_url .= 'redirect_uri=' . urlencode($redirect_uri);

    // Redirect to authorization url.
    drupal_add_js("top.location.href = '" . $auth_url . "'", 'inline');
  }

  // @TODO: Link accounts.
  if ( !user_is_logged_in() && $fbtab_app->link_accounts && $fbtab_app->request ) {
    if ( isset($fbtab_app->request['user_id']) && !empty($fbtab_app->request['user_id']) ) {
      if ( $user = fbtab_user_load_by_fbid($fbtab_app->request['user_id']) ) {
        if ( fbtab_user_authenticate($user) ) {
          watchdog('fbtab', 'Successfully authenticated in @name with @fbid', array('@name' => $user->name, '@fbid' => $fbtab_app->request['user_id']), WATCHDOG_NOTICE, l('view', 'user/' . $user->uid));
        }
        else {
          watchdog('fbtab', 'Failed to authenticate @name with @fbid', array('@name' => $user->name, '@fbid' => $fbtab_app->request['user_id']), WATCHDOG_WARNING, l('view', 'user/' . $user->uid));
        }
      }
    }
  }
}

/**
 * Load User by Facebook User ID.
 */
function fbtab_user_load_by_fbid($fbid) {
  $uid = db_select('fbtab_users', 'f')
    ->fields('f', array('uid'))
    ->condition('fbid', $fbid)
    ->execute()
    ->fetchField();

  return $uid ? user_load($uid) : FALSE;
}

/**
 * Login a Drupal Account.
 */
function fbtab_user_authenticate($account) {
  global $user;

  if ( $account->status ) {
    $form_state['uid'] = $account->uid;
    user_login_submit(array(), $form_state);
  }
  else {
    drupal_set_message(t('The username %name has not been activated or is blocked.', array('%name' => $account->name)), 'error');
  }

  return !empty($user->uid);
}

/**
 * Map Facebook Locale w/ Drupal language code.
 */
function fbtab_language_map($lang) {
  $map = array(
   'af_ZA' => 'af',
   'ar_AR' => 'ar',
   'ay_BO' => 'ay',
   'az_AZ' => 'az',
   'be_BY' => 'be',
   'bg_BG' => 'bg',
   'bn_IN' => 'bn',
   'bs_BA' => 'bs',
   'ca_ES' => 'ca',
   'cs_CZ' => 'cs',
   'cy_GB' => 'cy',
   'da_DK' => 'da',
   'de_DE' => 'de',
   'el_GR' => 'el',
   'en_US' => 'en',
   'eo_EO' => 'eo',
   'es_ES' => 'es',
   'et_EE' => 'et',
   'eu_ES' => 'eu',
   'fa_IR' => 'fa',
   'fi_FI' => 'fi',
   'fo_FO' => 'fo',
   'fr_CA' => 'fr',
   'fr_FR' => 'fr',
   'ga_IE' => 'ga',
   'gl_ES' => 'gl',
   'gn_PY' => 'gn',
   'gu_IN' => 'gu',
   'he_IL' => 'he',
   'hi_IN' => 'hi',
   'hr_HR' => 'hr',
   'hu_HU' => 'hu',
   'hy_AM' => 'hy',
   'id_ID' => 'id',
   'is_IS' => 'is',
   'it_IT' => 'it',
   'ja_JP' => 'ja',
   'jv_ID' => 'jv',
   'ka_GE' => 'ka',
   'kk_KZ' => 'kk',
   'km_KH' => 'km',
   'kn_IN' => 'kn',
   'ko_KR' => 'ko',
   'ku_TR' => 'ku',
   'la_VA' => 'la',
   'lt_LT' => 'lt',
   'lv_LV' => 'lv',
   'mg_MG' => 'mg',
   'mk_MK' => 'mk',
   'ml_IN' => 'ml',
   'mn_MN' => 'mn',
   'mr_IN' => 'mr',
   'ms_MY' => 'ms',
   'mt_MT' => 'mt',
   'nb_NO' => 'nb',
   'ne_NP' => 'ne',
   'nl_BE' => 'nl',
   'nn_NO' => 'nn',
   'pa_IN' => 'pa',
   'pl_PL' => 'pl',
   'ps_AF' => 'ps',
   'qu_PE' => 'qu',
   'rm_CH' => 'rm',
   'ro_RO' => 'ro',
   'ru_RU' => 'ru',
   'sa_IN' => 'sa',
   'se_NO' => 'se',
   'sk_SK' => 'sk',
   'sl_SI' => 'sl',
   'so_SO' => 'so',
   'sq_AL' => 'sq',
   'sr_RS' => 'sr',
   'sv_SE' => 'sv',
   'sw_KE' => 'sw',
   'ta_IN' => 'ta',
   'te_IN' => 'te',
   'tg_TJ' => 'tg',
   'th_TH' => 'th',
   'tl_ST' => 'tl',
   'tr_TR' => 'tr',
   'tt_RU' => 'tt',
   'uk_UA' => 'uk',
   'ur_PK' => 'ur',
   'uz_UZ' => 'uz',
   'vi_VN' => 'vi',
   'xh_ZA' => 'xh',
   'yi_DE' => 'yi',
   'zh_CN' => 'zh-hans',
   'zh_TW' => 'zh-hant',
   'zu_ZA' => 'zu',
  );

  return isset($map[$lang]) ? $map[$lang] : 'en';
}