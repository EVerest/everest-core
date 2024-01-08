import yaml


def test_currency_codes():
    with open("types/money.yaml") as f:
        money_content = yaml.safe_load(f)
        currencies_list = money_content["types"]["CurrencyCode"]["enum"]
        seen = set()
        non_unique = set()
        for code in currencies_list:
            assert len(code) == 3
            if code in seen:
                non_unique.add(code)
            else:
                seen.add(code)
        assert(len(non_unique) == 0), f"Repeating currency codes detected: {non_unique}"

